"""Synthetic EU/US metadata; no game files are distributed or executed."""
import contextlib
import io
import os
from pathlib import Path
import struct
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0,str(Path(__file__).resolve().parent))
import game_regions as region
from test_english_output import load_setup


def sfo(root, tid):
    path=Path(root)/'sce_sys/param.sfo';path.parent.mkdir(parents=True,exist_ok=True)
    keys=b'TITLE_ID\0APP_VER\0';values=tid.encode()+b'\0'+b'01.24\0'
    data=struct.pack('<5I',0x46535000,0x101,52,52+len(keys),2)
    data+=struct.pack('<HHIII',0,0x204,10,10,0)+struct.pack('<HHIII',9,0x204,6,6,10)+keys+values
    path.write_bytes(data);return path


class RegionTests(unittest.TestCase):
    def test_both_regions_prepare_and_renamed_folders_find_matching_update(self):
        setup=load_setup('preparar.py')
        with tempfile.TemporaryDirectory() as temp:
            for tid in region.SUPPORTED_TITLE_IDS:
                root=Path(temp)/tid;game=root/'renamed';update=root/(tid+'-patch')
                sfo(game,tid);sfo(update,tid);(game/'eboot.bin').write_bytes(b'synthetic')
                data=update/'data/chunk1';data.mkdir(parents=True);(data/'PATCH.DAT').write_bytes(b'')
                self.assertEqual(Path(region.find_patch(game)),update)
                with contextlib.redirect_stdout(io.StringIO()):self.assertEqual(setup.check_game(game,update)['TITLE_ID'],tid)

    def test_mismatched_update_is_not_discovered_and_explicit_selection_is_rejected(self):
        setup=load_setup('preparar.py')
        with tempfile.TemporaryDirectory() as temp:
            game=Path(temp)/'CUSA00935';update=Path(temp)/'CUSA00935-patch'
            sfo(game,'CUSA00935');sfo(update,'CUSA01176');(game/'eboot.bin').write_bytes(b'x')
            self.assertEqual(region.find_patch(game),'')
            with contextlib.redirect_stdout(io.StringIO()),self.assertRaises(SystemExit):setup.check_game(game,update)

    def test_dlc_auto_discovery_uses_metadata_and_never_falls_back_to_other_region(self):
        with tempfile.TemporaryDirectory() as temp,patch.dict(os.environ,{'APPDATA':temp}):
            game=Path(temp)/'renamed';sfo(game,'CUSA01176')
            other=Path(temp)/'shadPS4/addcont/CUSA00935';other.mkdir(parents=True)
            self.assertEqual(region.default_addcont(game),'')
            own=other.with_name('CUSA01176');own.mkdir();self.assertEqual(Path(region.default_addcont(game)),own)
            with patch.dict(os.environ,{'APPDATA':''}):self.assertEqual(region.default_addcont(game),'')

    def test_unknown_title_and_malformed_metadata_are_refused(self):
        with tempfile.TemporaryDirectory() as temp:
            file=sfo(temp,'CUSA99999');self.assertEqual(region.title_id(temp),'')
            valid=sfo(temp,'CUSA00935').read_bytes()
            for data in (b'',valid[:20],valid[:40],valid[:8]+struct.pack('<I',0xffffffff)+valid[12:],valid[:16]+struct.pack('<I',513)+valid[20:]):
                file.write_bytes(data)
                self.assertEqual(region.title_id(temp),'')
                self.assertEqual(region.find_patch(temp),'')


if __name__=='__main__':unittest.main()
