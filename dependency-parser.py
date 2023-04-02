import re
import subprocess


ldd_output = subprocess.Popen("ldd bin/libs3.so", shell=True, stdout=subprocess.PIPE).stdout.read().decode()

libfiles = []

for line in ldd_output.split('\n'):
    print(line)
    try:
        libf = re.search(r'(\S+) \(0x', line).group(1)
        # libfiles.append(libf)
    except:
        continue

    apt_file_output = subprocess.Popen(f"apt-file search {libf}", shell=True, stdout=subprocess.PIPE).stdout.read().decode()
    if not apt_file_output:
        continue
    print(apt_file_output)

    package_name = re.search('^([^:]+)', apt_file_output).group(1)

    apt_show = subprocess.Popen(f"apt show {package_name}", shell=True, stdout=subprocess.PIPE).stdout.read().decode()
    package_version = re.search(r"Version: (\S+)", apt_show).group(1)

    libfiles.append(f'{package_name} (>={package_version})')


print(' '.join(libfiles))
