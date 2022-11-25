Mathis Brossier
Pierre Surer
Assignment 1

For this assignment, we haven't faced any trouble. We have decided to use Cmake to compile the project.

The weird image that you can see in the results is due to a unormalized light vector plus a negative dot product creating these weird shadows.

To compute specular reflections, we have decided to implement the Blinn Phong model.

Finally, we decided to implement a light intensity model that allows us to put different lights with different intensities but also to take the distance into account. For this, the power values have been chosen arbitrarily.