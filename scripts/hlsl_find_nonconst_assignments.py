# Run from the workspace root.

import glob

hlsl_files = glob.glob("src/**/*.hlsl", recursive=True)
for hlsl_file in hlsl_files:
    print("#", hlsl_file)
    with open(hlsl_file, "r") as f:
        lines = f.readlines()
    lines = [line.strip() for line in lines]
    lines = [line for line in lines if line]
    lines = [line for line in lines if not line.startswith("#")]
    lines = [line for line in lines if not line.startswith("//")]
    lines = [line for line in lines if not line.startswith("if (")]
    lines = [line for line in lines if not line.startswith("for (")]
    lines = [line for line in lines if "=" in line]
    lines = [line for line in lines if line[0].isupper()]

    for line in lines:
        print(f"  {line}")
