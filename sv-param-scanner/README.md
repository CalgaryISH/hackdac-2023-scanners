# Parameter Scanner

## Description

Searches for matching parameter identifiers between parent and child.

For each module, we check the module instantiations inside and find matching parameters.
If we find a match, we check that the expression being assigned to the instantiation is the parameter identifier. 
The intuition is that parameters are often passed down the hierarchy of the design.

## instructions

```bash
make
./scanner <path_to_opentitan/hw/ip> &> res.txt
```