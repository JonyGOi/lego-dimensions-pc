"""Check worker limits before the extractor opens inputs or creates outputs."""
import argparse
from pathlib import Path
import subprocess
import tempfile


def main():
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);args=parser.parse_args()
    executable=str(Path(args.exe).resolve())
    with tempfile.TemporaryDirectory(prefix='pkg-jobs-') as temporary:
        root=Path(temporary);destination=root/'not-created';missing=root/'missing.pkg'
        for value in ('0','9','-1','abc','1x','999999999999999999999'):
            result=subprocess.run([executable,'--jobs',value,'--destino',str(destination),str(missing)],capture_output=True,text=True)
            if result.returncode!=2 or '--jobs requires' not in result.stdout or destination.exists():
                raise RuntimeError('Invalid worker argument was not safely rejected: '+value)
        result=subprocess.run([executable,'--jobs'],capture_output=True,text=True)
        if result.returncode!=2 or '--jobs requires' not in result.stdout:raise RuntimeError('Missing worker count was not rejected')
        for value in ('1','8'):
            result=subprocess.run([executable,'--jobs',value,'--destino',str(destination),str(missing)],capture_output=True,text=True)
            if result.returncode!=1 or 'file not found' not in result.stdout or destination.exists():
                raise RuntimeError('Valid worker limit did not reach ordinary input validation: '+value)
    print('Worker limits, missing values and non-existent inputs: passed (no output created)')


if __name__=='__main__':main()
