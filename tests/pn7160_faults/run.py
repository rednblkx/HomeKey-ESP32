"""Exercise production reader methods with a scripted NCI engine, without an SDK."""
import os
from pathlib import Path
import shlex
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
source = (root / "main/Pn7160Reader.cpp").read_text()
pairs = [("bool Pn7160Reader::pollForTag(", "bool Pn7160Reader::isTagStillPresent()"),
         ("bool Pn7160Reader::isTagStillPresent()", "void Pn7160Reader::releaseTag()"),
         ("void Pn7160Reader::releaseTag()", "void Pn7160Reader::endDiscovery()"),
         ("bool Pn7160Reader::healthCheck()", "bool Pn7160Reader::updateECP()")]
methods = []
for start, end in pairs:
    first = source.index(start)
    methods.append(source[first:source.index(end, first)])
with tempfile.TemporaryDirectory(prefix="pn7160-faults-") as temp:
    path = Path(temp)
    (path / "reader_methods.inc").write_text("\n".join(methods))
    executable = str(path / "test")
    subprocess.run([
        *shlex.split(os.environ.get("CXX", "c++")), "-std=c++17", "-O1", "-UNDEBUG",
        "-fsanitize=address,undefined", "-I" + str(path),
        str(root / "tests/pn7160_faults/reader_test.cpp"), "-o", executable,
    ], check=True)
    subprocess.run([executable], check=True)
