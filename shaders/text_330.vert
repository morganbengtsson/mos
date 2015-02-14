#version 330 core
uniform mat4 model_view_projection;
uniform mat4 model_view;
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 uv;

out vec2 v_position;
out vec2 v_uv;

void main(){
    v_position = (model_view * vec4(position, 0.0)).xy;
    v_uv = uv;
    //v_uv = (v_position.xy+vec2(1,1))/500.0;
    gl_Position = model_view_projection * vec4(position, 1.0);
};
