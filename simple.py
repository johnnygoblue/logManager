import subprocess

cmd = ["echo", "'some simple test'", ">>", "file"]

subprocess.Popen(cmd)
