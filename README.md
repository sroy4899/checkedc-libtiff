# Checked C Libtiff Conversion

This repository is dedicated to the testing of the Checked-C conversion tool available [here](https://github.com/plum-umd/checkedc-clang/tree/BigRefactor). 

This repository contains several branches. This branch (**master**) will have C code and relevant documentation.   

[Baseline](https://github.com/sroy4899/checkedc-libtiff/tree/baseline) represents this same C Code, but refactored in order to mimic the structure of [a similar repo for testing vsftpd](https://github.com/plum-umd/checkedc-eval-vsftpd/blob/re-port/README.md). It includes scripts for running the conversion tool and replacing the converted files. 

[Sane-baseline](https://github.com/sroy4899/checkedc-libtiff/sane-baseline) represents code from **Baseline** that has been modified in order to make conversion work. (EX. pruning features that are not yet supported by the tool that are making it crash, etc.). As the tool evolves and these features are supported, changes in **Sane-baseline** will be reverted/modified. 

[Baseline-converted](https://github.com/sroy4899/checkedc-libtiff/baseline-converted) represents code from **Baseline** that has been fully converted with the latest changes from the conversion tool. Commits to this branch are hashed by the relevant commit from the tool. Conversion on this branch is run without `-alltypes`, so will only support `_Ptr` conversions. 

Similarly, [Sane-baseline-converted](https://github.com/sroy4899/checkedc-libtiff/sane-baseline-converted) represents code from **Sane-baseline** that has been converterted with the `-alltypes` flag enabled, thereby supporting `_Array_ptr`, `_Nt_array_ptr`, and `_Checked` annotations. 

In order to test the tool, checkout either **Baseline** or **Sane-baseline**, run the tool, and run the script `diff.py` in order to get accurate information about the changes that have happened.
