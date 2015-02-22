Multi-EMMPM {#multiemmpmfilter}
=====

## Group (Subgroup) ##
Filter Group (Filter Subgroup)


## Description ##
This filter performs the EM/MPM aglorithm on an entire AttributeMatrix worth of DataArrays. The user needs to select the AttributeMatrix that contains the image data (8 bit, unsigned char only) data and the filter will perform EM/MPM on each data array. The segmented images will be stored into a newly created AttributeMatrix where the name of each output array will be the a user defined **prefix** + the original name of the input array.

The filter author highly recommends reading the original paper on the subject to gain an understanding of the values for the input parameters.

There is also an option to use the last Mu (mean) and Sigma (variance) values calculated on the current array as the initialization values for the next data array to process. Using this can help the EM/MPM algorithm achieve subjectively "better" segmentations by starting the algorithm at values that should be close to the ending values. **NOTE** This option should _only_ be used if all of the images are similar to one another. Something like a montage/tiled data set or a 3D stack of images. If the input arrays are vastly different using this option can have negative effects on the accuracy of the final segmented images.

## Parameters ##
| Name             | Type |
|------------------|------|
| Input Attribute Matrix | Data Array Path |
| Num Classes | Integer |
| Exchange Energy | float |
| Histogram (EM) Loops | int |
| Segmentation (EMPM) Loops | int |
| Use Simulated Annealing | bool |
| Use Gradient Penalty | bool |
| Use Curvature Penalty | bool |
| Output AttributeMatrix Name | String |
| Use Mu/Sigma as initializers | Bool |


## Required Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Unsigned 8 Bit Integers  | NONE | Input Image Data  | 8 Bit Grayscale images only |


## Created Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Unsigned 8 Bit Integers  | _PREFIX_Arrayname           |    |    |



## Authors ##

**Copyright** 2015 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.


## Citations ##
H. C. Chuang, M. L. Comer, and J. Simmons, “Texture Segmentation in Microstructure Images of Advanced Materials,” In Proceedings of the _2008 IEEE Southwest Symposium on Image Analysis and Interpretation_, March 24-26, 2008, Santa Fe, NM

See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)