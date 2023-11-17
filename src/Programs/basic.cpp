#include <iostream>

#include "context.h"
#include "program.h"

bool BasicProgram::load() {
  long long int pos_offset = 0;
  long long int nor_offset = 12;
  long long int tex_offset = 24; 
  long long int element_size = sizeof(float);
  programId = quickCreateProgram(vertProgramFile, fragProgramFIle);
  vector<float> temp_buffer;

  int num_model = (int)ctx->models.size();
  VAO = new GLuint[num_model];
  glGenVertexArrays(num_model, VAO);
  for (int i = 0; i < num_model; i++) {
    glBindVertexArray(VAO[i]);
    Model* model = ctx->models[i];

    GLuint VBO[1];
    glGenBuffers(1, VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, (long long int)32 * model->numVertex, (void*)0, GL_STATIC_DRAW);

   pos_offset = 0;
   nor_offset = 12;
   tex_offset = 24; 
    for (int j = 0; j < model->numVertex  ; j++ ) {
      glBufferSubData(GL_ARRAY_BUFFER, pos_offset, element_size, &model->positions[3 * j]);
      glBufferSubData(GL_ARRAY_BUFFER, pos_offset + element_size, element_size, &model->positions[3 * j + 1]);  
      glBufferSubData(GL_ARRAY_BUFFER, pos_offset + element_size*2, element_size, &model->positions[3 * j + 2]);
      glBufferSubData(GL_ARRAY_BUFFER, nor_offset, element_size, &model->normals[3 * j]);
      glBufferSubData(GL_ARRAY_BUFFER, nor_offset + element_size, element_size, &model->normals[3 * j + 1]);
      glBufferSubData(GL_ARRAY_BUFFER, nor_offset + element_size * 2, element_size, &model->normals[3 * j + 2]);
      glBufferSubData(GL_ARRAY_BUFFER, tex_offset, element_size, &model->texcoords[2 * j]);
      glBufferSubData(GL_ARRAY_BUFFER, tex_offset + element_size, element_size, &model->texcoords[2 * j + 1]);
      pos_offset = pos_offset + 32; 
      nor_offset = nor_offset + 32;
      tex_offset = tex_offset + 32;
    } // for 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)24);
  } // for
  /* TODO#2-2: Pass model vertex data to vertex buffer
   *           1. Generate and bind vertex array object (VAO) for each model
   *           2. Generate and bind three vertex buffer objects (VBOs) for each model
   *           3. Pass model positions, normals and texcoords to VBOs
   * Note:
   *           1. You may refer to Programs/example.cpp and example.vert
   *           2. You may checkout basic.vert to get location index first
   *           3. You can get model data from ctx->models[i]->{posisions,normals,texcoords}
   * Keywords:
   *           - glGenVertexArrays
   *           - glBindVertexArray
   *           - glGenBuffers
   *           - glBufferData
   *           - glEnableVertexAttribArray
   *           - glVertexAttribPointer
   */

  return programId != 0;
}

void BasicProgram::doMainLoop() {
  glUseProgram(programId);
  int obj_num = (int)ctx->objects.size();
  for (int i = 0; i < obj_num; i++) {
    int modelIndex = ctx->objects[i]->modelIndex;
    glBindVertexArray(VAO[modelIndex]);

    Model* model = ctx->models[modelIndex];
    const float* p = ctx->camera->getProjectionMatrix();
    GLint pmatLoc = glGetUniformLocation(programId, "Projection");
    glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, p);

    const float* v = ctx->camera->getViewMatrix();
    GLint vmatLoc = glGetUniformLocation(programId, "ViewMatrix");
    glUniformMatrix4fv(vmatLoc, 1, GL_FALSE, v);

    const float* m = glm::value_ptr(ctx->objects[i]->transformMatrix * model->modelMatrix);
    GLint mmatLoc = glGetUniformLocation(programId, "ModelMatrix");
    glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, m);

    glUniform1i(glGetUniformLocation(programId, "ourTexture"), 0);
    glBindTexture(GL_TEXTURE_2D, model->textures[ctx->objects[i]->textureIndex]);
    glDrawArrays(model->drawMode, 0, model->numVertex);
    glBindVertexArray(0);
  }
  glUseProgram(0);

  /* TODO#2-3: Render objects with shader
   *           1. use and bind program (BasicProgram::programId)
   *           2. Iterate all objects (ctx->objects)
   *           3. Load currect model VAO for object
   *           4. Pass projection, view, model matrix to shaders
   *           5. Pass model texture to shaders
   *           6. Draw with glDrawArrays
   * Note:
   *           1. You may refer to Programs/example.cpp and example.vert
   *           2. You may checkout definition of Context class, Model class and Object class first
   *           3. You can get object class from ctx->objects[i]
   *           4. You can get texture index of Model::textures for current object by Object::textureIndex
   *           5. you may also need model->drawMode, model->numVertex
   * Keywords:
   *           - glUseProgram
   *           - glBindVertexArray
   *           - glGetUniformLocation
   *           - glUniformMatrix4fv
   *           - glActiveTexture
   *           - glBindTexture
   *           - glUniform1i
   *           - glDrawArrays
   */

}