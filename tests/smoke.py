"""Exercise shell input handling without modifying user files."""
import subprocess


def run(source):
    return subprocess.run(["./build/shell"], input=source, text=True,
                          capture_output=True, timeout=5)


assert run("").returncode == 0
assert run("\n   \nexit\n").returncode == 0
result = run("echo " + "x" * 3000 + "\nexit\n")
assert result.returncode == 0
assert "x" * 3000 in result.stdout
result = run("command_that_does_not_exist_92317\necho still-running\nexit\n")
assert result.returncode == 0
assert result.stdout.count("still-running") == 1
print("Shell input smoke tests passed.")
