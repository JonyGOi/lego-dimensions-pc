"""Asset-free compatibility for compiled-pair metadata used by AOT preparation."""
import importlib.util
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE/'nu2_shader_extractor'))
spec=importlib.util.spec_from_file_location('metadata_extractor',HERE/'nu2_shader_extractor/nu2_shader_extractor.py')
extractor=importlib.util.module_from_spec(spec);spec.loader.exec_module(extractor)


class ShaderMetadataTests(unittest.TestCase):
    def test_prepared_legacy_and_early_beta_pairs_are_both_recognized(self):
        with tempfile.TemporaryDirectory() as temp:
            folder=Path(temp)/'pairs';folder.mkdir()
            expected=set()
            for i,flag in enumerate(('sim','yes','nao','no','similar')):
                key=(f'{i+1:016x}','0123456789abcdef','0011223344556677')
                header=f'pair vs {key[0]} ps {key[1]} variante {key[2]} fetch_sgpr 0 real {flag}\n'
                (folder/('pair_'+'_'.join(key)+'.txt')).write_text(header,encoding='ascii')
                if flag in ('sim','yes'):expected.add(key)
            with patch.object(extractor,'SPV',str(Path(temp))):found=extractor.known_pairs()
            self.assertEqual(set(found),expected)

    def test_human_readable_english_is_not_a_serialized_specialization_flag(self):
        with tempfile.TemporaryDirectory() as temp:
            folder=Path(temp)/'pairs';folder.mkdir()
            (folder/'pair_invalid.txt').write_text('pair vs aa ps bb variante cc fetch_sgpr 0 actual yes\n',encoding='ascii')
            with patch.object(extractor,'SPV',temp):self.assertEqual(extractor.known_pairs(),{})


if __name__=='__main__':unittest.main()
