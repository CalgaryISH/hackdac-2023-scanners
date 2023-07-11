import argparse
import re

def displaymatch(match):
    if match is None:
        return None
    return '<Match: %r, groups=%r>' % (match.group(), match.groups())

def main():
    # argParser = argparse.ArgumentParser("Scanner")
    # argParser.add_argument("inputFiles", nargs="+")
    # args = argParser.parse_args()
    
    insecureAssign = re.compile(r"(reg_rdata_next.+)=(.*key_share(?:0|1).*);", re.M)
    with open("reg_top.txt") as fileSet:
            # print(inputFile.read())
            inputFiles = fileSet.readlines()
            for inputFile in inputFiles:
                print(f'scanning file {inputFile}')
                with open(inputFile.strip()) as f:
                    # print(inputFile.read())
                    lines = f.readlines()
                    lineno = 1
                    for line in lines:
                        if insecureAssign.search(line) != None:
                            print(f'Insecure assignment in line {lineno}: {line.strip()}')
                        lineno += 1


if __name__=="__main__":
    main()