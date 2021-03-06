//  Pipeline.cpp
//  ebib_demo
//  Created by Ishmeet Singh Kohli on 06/09/17.
//

#include "Utils.hpp"
#include <Pipeline.hpp>

#define LOG(x) std::cout << x << std::endl
using namespace std;

typedef Tensor<float, 3> Tensor3f;

Pipeline::Pipeline(QOpenGLShaderProgram *program) {
  m_template = new Template(program);
  m_regProcessor = new RegistrationProcessor(m_template);
  m_opGenerator = new OperatorGenerator(m_template);
  m_volGenerator = new VolumeGenerator();
  userPoints = new UserPoints(program);
  S = 100;

  MatrixXf X = RowVectorXf::LinSpaced(S, MIN[0] - 0.2, MAX[0] + 0.2).replicate(S, 1);
  MatrixXf Y = VectorXf::LinSpaced(S, MIN[1] - 0.2, MAX[1] + 0.2).replicate(1, S);

  circle1 = new Circle(0, -0.35, 0.25, 0.6, X, Y);
  circle2 = new Circle(0, 0.35, 0.25, 0.6, X, Y);

  //  result = new Result(X, Y, program);
  result3D = new Result3D(S, program);
}

void Pipeline::initializeSpheres() {
  sphere1 = new Sphere(circle1->center[0], circle1->center[1], 0, circle1->r1, circle1->r2, S);
  sphere2 = new Sphere(circle2->center[0], circle2->center[1], 0, circle2->r1, circle2->r2, S);
}

void Pipeline::map(vector<Vector2f> &samples) {
  Map<MatrixXf> samples_eigen(samples.data()->data(), 2, samples.size());

  MatrixXf f1, f2, g1[2], g2[2];
  MatrixXf X = samples_eigen.row(0);
  MatrixXf Y = samples_eigen.row(1);

  circle1->calculateDistanceField(X, Y, f1, g1);
  circle2->calculateDistanceField(X, Y, f2, g2);
  MatrixXf alpha = calculateGradientAngles(g1, g2);
  MatrixXf templatePoints(3, samples_eigen.cols());
  templatePoints.row(0) = f1;
  templatePoints.row(1) = f2;
  templatePoints.row(2) = alpha;
  userPoints->setUserPoints(templatePoints);
}

void Pipeline::registerPoints() {
  MatrixXf processedSamples = preprocessSamples(userPoints->m_userPoints);
  m_regProcessor->registerPoints(processedSamples);
}

void Pipeline::generate(QOpenGLShaderProgram *program) {
  result3D = new Result3D(S, program);
//  long int before = mach_absolute_time();
  Tensor3f G = m_opGenerator->generateOperator(50);
  //  LOG("Step 1, Time taken " << ((mach_absolute_time() - before) * 100) / (1000 * 1000 * 1000) / 100.0);
  initializeSpheres();
  //  LOG("Step 2, Time taken " << ((mach_absolute_time() - before) * 100) / (1000 * 1000 * 1000) / 100.0);
  Tensor3f alpha = calculateGradientAngles(sphere1->gradient, sphere2->gradient);
  //  LOG("Step 3, Time taken " << ((mach_absolute_time() - before) * 100) / (1000 * 1000 * 1000) / 100.0);
  Tensor3f distanceField = m_volGenerator->generate(sphere1->distanceField, sphere2->distanceField, alpha, G);
  //  LOG("Step 4, Time taken " << ((mach_absolute_time() - before) * 100) / (1000 * 1000 * 1000) / 100.0);
  result3D->setDistanceField(distanceField);
//  LOG("Time taken for generation : " << ((mach_absolute_time() - before) * 100) / (1000 * 1000 * 1000) / 100.0);
}

void Pipeline::automate(vector<Vector2f> &samples, QOpenGLShaderProgram *program) {
  long int before = mach_absolute_time();
  map(samples);
  MatrixXf processedSamples = preprocessSamples(userPoints->m_userPoints);
  m_regProcessor->automaticRegistration(processedSamples);
  generate(program);
  LOG("Total time taken : " << ((mach_absolute_time() - before) * 100) / (1000 * 1000 * 1000) / 100.0);
}

MatrixXf Pipeline::calculateGradientAngles(MatrixXf (&g1)[2], MatrixXf (&g2)[2]) {
  MatrixXf mag1 = (g1[0].array().pow(2) + g1[1].array().pow(2)).sqrt();
  MatrixXf mag2 = (g2[0].array().pow(2) + g2[1].array().pow(2)).sqrt();
  MatrixXf dotP = ((g1[0].array() * g2[0].array()) + (g1[1].array() * g2[1].array())) / (mag1.array() * mag2.array());
  MatrixXf alpha = dotP.array().acos() / M_PI;
  return alpha;
}

Tensor<float, 3> Pipeline::calculateGradientAngles(Tensor<float, 3> (&g1)[3], Tensor<float, 3> (&g2)[3]) {
  int res = g1[0].dimension(0);
  Tensor<float, 3> alpha(res, res, res);
  for (int i = 0; i < res; i++) {
    for (int j = 0; j < res; j++) {
      for (int k = 0; k < res; k++) {
        float mag1 = sqrt(pow(g1[0](i, j, k), 2) + pow(g1[1](i, j, k), 2) + pow(g1[2](i, j, k), 2));
        float mag2 = sqrt(pow(g2[0](i, j, k), 2) + pow(g2[1](i, j, k), 2) + pow(g2[2](i, j, k), 2));
        float dotP = ((g1[0](i, j, k) * g2[0](i, j, k)) + (g1[1](i, j, k) * g2[1](i, j, k))) / (mag1 * mag2);
        alpha(i, j, k) = acos(dotP) / M_PI;
      }
    }
  }
  return alpha;
}

MatrixXf Pipeline::preprocessSamples(MatrixXf& samples) {
  if(samples.size() == 0) {
    return samples;
  }

  float threshold = 0.05;
  MatrixXf processed = igl::slice_mask(samples, samples.row(0).array() > threshold, 2);
  processed = igl::slice_mask(processed, processed.row(1).array() > threshold, 2);
  return processed;
}

void Pipeline::resetRegistration() {
  this->m_template->setup();
  this->userPoints->clear();
  this->m_regProcessor->reset();
  this->result3D->resetBuffers();
}
