#include "ObjectScene.hpp"

ObjectScene::ObjectScene(QOpenGLShaderProgram *program, Pipeline *pipeline)
/*: index_vbo{QOpenGLBuffer(QOpenGLBuffer::IndexBuffer),
                QOpenGLBuffer(QOpenGLBuffer::IndexBuffer)}*/ {
  this->m_program = program;
  this->m_pipeline = pipeline;

  initializeBuffers();
  initializeBoundary();
  initializeSamples();
  updateBuffers();
}

void ObjectScene::initializeSamples() {
  int n = 30;
  Vector2f p1 = m_pipeline->circle2->center;
  p1[0] -= m_pipeline->circle2->r1;
  Vector2f p2 = m_pipeline->circle1->center;
  p2[0] -= m_pipeline->circle1->r1;

  samples.resize(2, n * 2);
  samples << interpolatePoints(p1, p2, n), interpolateArc(m_pipeline->circle1->center, m_pipeline->circle1->r1, M_PI, 2 * M_PI, n);

  m_vbo[3].bind();
  m_vbo[3].setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_vbo[3].allocate(samples.data(), samples.size() * sizeof(GL_FLOAT));
  m_vao[3].bind();
  m_program->enableAttributeArray(0);
  m_program->setAttributeBuffer(0, GL_FLOAT, 0, 2);
  m_vbo[3].release();
  m_vao[3].release();
}

void ObjectScene::initializeBoundary() {
  boundary.resize(2, 4);
  boundary.col(0) << -0.99, -0.99;
  boundary.col(1) << 0.99, -0.99;
  boundary.col(2) << 0.99, 0.99;
  boundary.col(3) << -0.99, 0.99;

  m_vbo[2].bind();
  m_vbo[2].setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_vbo[2].allocate(boundary.data(), boundary.size() * sizeof(GL_FLOAT));
  m_vao[2].bind();
  m_program->enableAttributeArray(0);
  m_program->setAttributeBuffer(0, GL_FLOAT, 0, 2);
  m_vbo[2].release();
  m_vao[2].release();
}

void ObjectScene::render() {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  m_program->setUniformValue("camera", QMatrix4x4());

  m_vao[0].bind();
  m_program->setUniformValue("color", QVector4D(0.0f, 1.0f, 0.5f, 1.0f));
  glDrawArrays(GL_LINES, m_pipeline->circle1->triangles.size(), m_pipeline->circle1->line_segments.size());

  m_vao[1].bind();
  m_program->setUniformValue("color", QVector4D(1.0f, 0.5f, 0.5f, 1.0f));
  glDrawArrays(GL_LINES, m_pipeline->circle1->triangles.size(), m_pipeline->circle2->line_segments.size());

  m_vao[0].bind();
  m_program->setUniformValue("color", QVector4D(0.0f, 1.0f, 0.5f, 1.0f));
  glDrawArrays(GL_TRIANGLES, 0, m_pipeline->circle1->triangles.size());

  m_vao[1].bind();
  m_program->setUniformValue("color", QVector4D(1.0f, 0.5f, 0.5f, 1.0f));
  glDrawArrays(GL_TRIANGLES, 0, m_pipeline->circle2->triangles.size());

  m_vao[2].bind();
  m_program->setUniformValue("color", QVector4D(0.0f, 0.0f, 0.0f, 1.0f));
  glDrawArrays(GL_LINE_LOOP, 0, 4);

  m_vao[3].bind();
  m_program->setUniformValue("color", QVector4D(0.0f, 0.0f, 0.0f, 1.0f));
  glDrawArrays(GL_POINTS, 0, samples.cols());
}

void ObjectScene::initializeBuffers() {
  m_vbo[0].create();
  m_vbo[1].create();
  m_vao[0].create();
  m_vao[1].create();

  // boundary
  m_vbo[2].create();
  m_vao[2].create();

  // samples
  m_vbo[3].create();
  m_vao[3].create();

  //  index_vbo[0].create();
  //  index_vbo[1].create();
}

void ObjectScene::updateBuffers() {
  bindObject(0, m_pipeline->circle1);
  bindObject(1, m_pipeline->circle2);
}

void ObjectScene::bindObject(int i, Circle *circle) {
  m_vbo[i].bind();
  m_vbo[i].setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_vbo[i].allocate((circle->line_segments.size() + circle->triangles.size()) * 4 * sizeof(GL_DOUBLE));
  m_vbo[i].write(0, circle->triangles.data(), circle->triangles.size() * 4 * sizeof(GL_DOUBLE));
  m_vbo[i].write(circle->triangles.size() * 4 * sizeof(GL_DOUBLE), circle->line_segments.data(), circle->line_segments.size() * 4 * sizeof(GL_DOUBLE));
  m_vao[i].bind();
  m_program->enableAttributeArray(0);
  m_program->setAttributeBuffer(0, GL_DOUBLE, 0, 2);
  m_vbo[i].release();
  m_vao[i].release();
}

MatrixXf ObjectScene::interpolatePoints(Vector2f p1, Vector2f p2, int samples) {
  RowVectorXf X = RowVectorXf::LinSpaced(samples, p1[0], p2[0]);
  RowVectorXf Y = RowVectorXf::LinSpaced(samples, p1[1], p2[1]);
  MatrixXf points(2, samples);
  points << X, Y;
  return points;
}

MatrixXf ObjectScene::interpolateArc(Vector2f centre, float radius, float startAngle, float endAngle, int samples) {
  RowVectorXf angles = RowVectorXf::LinSpaced(samples, startAngle, endAngle);
  MatrixXf points(2, samples);
  for (int i = 0; i < samples; i++) {
    points.col(i) << radius * cos(angles(i)) + centre[0], radius * sin(angles(i)) + centre[1];
  }
  return points;
}
