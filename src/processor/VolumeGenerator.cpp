#include "VolumeGenerator.hpp"

VolumeGenerator::VolumeGenerator() {}

MatrixXf VolumeGenerator::generate(MatrixXf &df1, MatrixXf &df2, MatrixXf &alpha, Tensor3f &G) {
  MatrixXf F = interpolate3(df1, df2, alpha, G);
  return F;
}

Tensor3f VolumeGenerator::generate(Tensor3f &df1, Tensor3f &df2, Tensor3f &alpha, Tensor3f &G) {
  Tensor3f F = interpolate3(df1, df2, alpha, G);
  return F;
}

Tensor3f VolumeGenerator::interpolate3(Tensor3f &df1, Tensor3f &df2, Tensor3f &alpha, Tensor3f &G) {
  int size = G.dimension(0);

  Tensor3f F(df1.dimensions());
  VectorXf indices = VectorXf::LinSpaced(size, 0, 1);

  for (int i = 0; i < df1.dimension(0); i++) {
    for (int j = 0; j < df1.dimension(1); j++) {
      for (int k = 0; k < df1.dimension(2); k++) {
        int indexX = -1, indexY = -1, indexZ = -1;

        for (int idx = 0; idx < size - 1; idx++) {
          if (df1(i, j, k) >= indices(idx) && df1(i, j, k) < indices(idx + 1))
            indexX = idx;

          if (df2(i, j, k) >= indices(idx) && df2(i, j, k) < indices(idx + 1))
            indexY = idx;

          if (alpha(i, j, k) >= indices(idx) && alpha(i, j, k) < indices(idx + 1))
            indexZ = idx;

          if (indexX != -1 && indexY != -1 && indexZ != -1)
            break;
        }

        float xd = (df1(i, j, k) - indices(indexX)) / (indices(indexX + 1) - indices(indexX));
        float yd = (df2(i, j, k) - indices(indexY)) / (indices(indexY + 1) - indices(indexY));
        float zd = (alpha(i, j, k) - indices(indexZ)) / (indices(indexZ + 1) - indices(indexZ));

        float c00 = G(indexX, indexY, indexZ) * (1 - xd) + G(indexX + 1, indexY, indexZ) * xd;
        float c01 = G(indexX, indexY, indexZ + 1) * (1 - xd) + G(indexX + 1, indexY, indexZ + 1) * xd;
        float c10 = G(indexX, indexY + 1, indexZ) * (1 - xd) + G(indexX + 1, indexY + 1, indexZ) * xd;
        float c11 = G(indexX, indexY + 1, indexZ + 1) * (1 - xd) + G(indexX + 1, indexY + 1, indexZ + 1) * xd;

        float c1 = c01 * (1 - yd) + c11 * yd;
        float c0 = c00 * (1 - yd) + c10 * yd;

        F(i, j, k) = c0 * (1 - zd) + c1 * zd;
      }
    }
  }

  return F;
}

MatrixXf VolumeGenerator::interpolate3(MatrixXf &df1, MatrixXf &df2, MatrixXf &alpha, Tensor3f &G) {
  int size = G.dimension(0);

  MatrixXf F(df1.rows(), df1.cols());
  VectorXf indices = VectorXf::LinSpaced(size, 0, 1);

  for (int i = 0; i < df1.rows(); i++) {
    for (int j = 0; j < df1.cols(); j++) {
      int indexX = -1, indexY = -1, indexZ = -1;
      for (int idx = 0; idx < size - 1; idx++) {
        if (df1(i, j) >= indices(idx) && df1(i, j) < indices(idx + 1))
          indexX = idx;

        if (df2(i, j) >= indices(idx) && df2(i, j) < indices(idx + 1))
          indexY = idx;

        if (alpha(i, j) >= indices(idx) && alpha(i, j) < indices(idx + 1))
          indexZ = idx;

        if (indexX != -1 && indexY != -1 && indexZ != -1)
          break;
      }

      float xd = (df1(i, j) - indices(indexX)) / (indices(indexX + 1) - indices(indexX));
      float yd = (df2(i, j) - indices(indexY)) / (indices(indexY + 1) - indices(indexY));
      float zd = (alpha(i, j) - indices(indexZ)) / (indices(indexZ + 1) - indices(indexZ));

      float c00 = G(indexX, indexY, indexZ) * (1 - xd) + G(indexX + 1, indexY, indexZ) * xd;
      float c01 = G(indexX, indexY, indexZ + 1) * (1 - xd) + G(indexX + 1, indexY, indexZ + 1) * xd;
      float c10 = G(indexX, indexY + 1, indexZ) * (1 - xd) + G(indexX + 1, indexY + 1, indexZ) * xd;
      float c11 = G(indexX, indexY + 1, indexZ + 1) * (1 - xd) + G(indexX + 1, indexY + 1, indexZ + 1) * xd;

      float c1 = c01 * (1 - yd) + c11 * yd;
      float c0 = c00 * (1 - yd) + c10 * yd;

      F(i, j) = c0 * (1 - zd) + c1 * zd;
    }
  }

  return F;
}
