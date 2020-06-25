# Checked C Libtiff Conversion Benchmark

This branch is out-of-the-box C Code from Libtiff. In order to perform regression testing, please do the following:

1. Change the paths in `convert_all.sh` to match the location of the conversion tool on your machine. Also update the paths in `update_database.py`, `replace.py`, and `diff.py`.

2. Now run `diff.py`. This will automatically run `update_database.py` to update compile_commands.json, then run the tool, replace the relevant files, and generate descriptive diffs against the branch **Baseline-converted** about the changes the tool has brought about.
