# Device Tree for VEGA Secret Note2 (ef67)

## Spec Sheet

| Feature                 | Specification                     |
| :---------------------- | :-------------------------------- |
| CPU                     | Quad-core 2.5 GHz Krait 450        |
| Chipset                 | Qualcomm APQ8084 Snapdragon 805   |
| GPU                     | Adreno 420                        |
| Memory                  | 3 GB LPDDR3                       |
| Shipped Android Version | 4.4.2   |
| Storage                 | 32 GB                             |
| MicroSD                 | Up to 2 TB      |
| Battery                 | 3330 mAh (removable)              |
| Dimensions              | 154.65 x 82.4 x 8.95 mm           |
| Display                 | 2560 x 1440 pixels, 6.0" IPS LCD   |
| Rear Camera             | 13 MP, Sony IMX214, OIS, AF, LED flash |
| Front Camera            | 2.1 MP, Samsung S5K6B2YX          |
| Release Date            | Not released                     |

## Patches

From the Android source root, apply once to unmodified CM11 checkouts:

```sh
git -C system/core am "$PWD"/device/pantech/ef67/patches/system/core/*.patch
git -C hardware/qcom/display-caf am "$PWD"/device/pantech/ef67/patches/hardware/qcom/display-caf/*.patch
git -C hardware/qcom/media am "$PWD"/device/pantech/ef67/patches/hardware/qcom/media/*.patch
```
