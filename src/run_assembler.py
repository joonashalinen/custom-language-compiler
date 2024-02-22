import sys
sys.path.insert(0, 'libraries')
from assembler import assemble

def runAssember():
    assert len(sys.argv) == 3
    assemblyFile = open(sys.argv[1], encoding='utf-8')
    assembly = assemblyFile.read()
    print(assembly)
    assemble(assembly, sys.argv[2], "files")

runAssember()