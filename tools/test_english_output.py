"""Asset-free checks for English setup output and legacy progress compatibility."""
import contextlib
import importlib.util
import io
import os
from pathlib import Path
import subprocess
import sys
import unittest
from unittest.mock import patch

ROOT=Path(__file__).resolve().parents[1]


def load_setup(name):
    spec=importlib.util.spec_from_file_location('test_'+name,ROOT/'tools/setup'/name)
    module=importlib.util.module_from_spec(spec)
    with patch.dict(os.environ,{'LD_LANG':'en'}):spec.loader.exec_module(module)
    return module


class EnglishOutputTests(unittest.TestCase):
    def test_default_is_english_and_explicit_portuguese_remains_available(self):
        module=load_setup('preparar.py')
        with patch.dict(os.environ,{},clear=True):self.assertFalse(module._portuguese())
        with patch.dict(os.environ,{'LD_LANG':'pt'}):self.assertTrue(module._portuguese())
        self.assertEqual(load_setup('mods.py').T('portugues','English'),'English')
        self.assertEqual(load_setup('ld_transfer.py').LANG,'en')

    def test_english_and_legacy_batch_lines_keep_progress_reporting(self):
        module=load_setup('preparar.py')
        class Process:
            returncode=0
            def __init__(self,line):self.stdout=iter([line])
            def wait(self):return 0
        for line in ('  batch 1/2: 128 pairs packed','  lote 1/2: 128 pares empacotados'):
            output=io.StringIO()
            with patch.object(module.subprocess,'Popen',return_value=Process(line)),contextlib.redirect_stdout(output):
                module.run_step([],'Compiling',10,90)
            self.assertIn('PROGRESSO 50 Compiling (1/2)',output.getvalue())

    def test_help_is_english_and_existing_command_names_are_preserved(self):
        cases=(('preparar.py','separate update folder',('--patch','--sem-aot')),
               ('mods.py','extract a mod',('--instalar','--carregador')),
               ('ld_transfer.py','folder for backing up saves',('--origem','--destino')))
        for filename,text,options in cases:
            result=subprocess.run([sys.executable,str(ROOT/'tools/setup'/filename),'--help'],
                capture_output=True,text=True,encoding='utf-8',env={**os.environ,'LD_LANG':'en','PYTHONIOENCODING':'utf-8'})
            self.assertEqual(result.returncode,0,result.stderr)
            self.assertIn(text,result.stdout)
            for option in options:self.assertIn(option,result.stdout)


if __name__=='__main__':unittest.main()
