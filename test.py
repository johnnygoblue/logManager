import multiprocessing
import os
import subprocess

cwd = os.getcwd()
valid_cmds = ['b', 'c', 'd', 'e', 'k', 'q', 't', 'g']

class TestSample:
    def __init__(self, cmd=None):
        self.exec_file = os.path.join(cwd, "logman")
        self.input_file = "sample-log.txt"
        #self.input_file = os.path.join(cwd, "sample-log.txt")
        self.cmd_file = "sample-" + cmd + "-cmds.txt"
        #self.cmd_file = os.path.join(cwd, "sample-" + cmd + "-cmds.txt")
        self.output_file = "sample-" + cmd
        #self.output_file = os.path.join(cwd, "sample-" + cmd)
        self.golden_file = "sample-" + cmd + "-out.txt"
        #self.golden_file = os.path.join(cwd, "sample-" + cmd + "-out.txt")
        # run this test instance from below
        print("instance with cmd = ", cmd)
        self._run_test()
        #self._diff_test()
        #self._print_result()

    def _run_test(self):
        cmd = [self.exec_file, self.input_file, "<", self.cmd_file, ">", self.output_file]
        #self.output = subprocess.check_output(cmd, stderr=subprocess.STDOUT, timeout=3)
        subprocess.Popen(["ls", "-l", ">>", "tmp"])
        #subprocess.Popen(cmd)

    def _diff_test(self):
        cmd = ["diff", self.golden_file, self.output_File]
        self.result = subprocess.run(cmd, stdout=subprocess.PIPE, timeout=3).stdout.decode('utf-8')

    def _print_result(self):
        print("RESULT")
        if self.result.stdout is None:
            print("command %s PASS" %(cmd))
        else:
            print("command %s FAIL" %(cmd))


class TestP3:
    def __init__(self, test_cmds=None):
        if test_cmds is not None:
            self.cmds = test_cmds

    def run_sample_test_cmds(self):
        [TestSample(c) for c in self.cmds if c in valid_cmds]

    def test_b_command(self):
        return None

    def test_c_command(self):
        return None

    def test_d_command(self):
        return None

    def test_e_command(self):
        return None

    def test_k_command(self):
        return None

    def test_q_command(self):
        return None

    def test_t_command(self):
        return None


def main():
    test_cmds = ['b', 'c', 'd', 'e', 'q', 't']
    test = TestP3(test_cmds)
    test.run_sample_test_cmds()

if __name__ == "__main__":
    main()
