//
//  UserPoints.cpp
//  ebib_demo
//
//  Created by Ishmeet Singh Kohli on 13/07/17.
//
//

#include "UserPoints.hpp"

using namespace ebib;
using namespace Eigen;

UserPoints::UserPoints(tdogl::Program& gProgram){
    this->gProgram = &gProgram;
    mUserPoints = MatrixXf(3, NUM_OF_USER_POINTS);
    glGenBuffers(1, &gVBO);
    
    float RADIUS = 0.5f;
    
    for (int i = 0; i < NUM_OF_USER_POINTS; i++) {
        double angle_rad = 0.3 + i * PI/2 / NUM_OF_USER_POINTS / 1.4;
        mUserPoints.col(i) = Vector3f(RADIUS * glm::sin(angle_rad),RADIUS * glm::cos(angle_rad),0.5f);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);
    glBufferData(GL_ARRAY_BUFFER, mUserPoints.size() * sizeof(GLfloat), mUserPoints.data(), GL_STATIC_DRAW);
    
}

void UserPoints::render(){
    glEnableVertexAttribArray(0);
    gProgram->setUniform("myColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDrawArrays(GL_POINTS, 0, NUM_OF_USER_POINTS);

}