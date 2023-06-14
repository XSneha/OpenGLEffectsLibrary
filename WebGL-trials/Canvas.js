var canvas = null;
var gl = null;
var canvas_original_width;
var canvas_original_height;
var bFullScreen = false;

const webGLMacros = {SCS_ATTRIBUTE_POSITION : 1,
                     SCS_ATTRIBUTE_COLOR : 2,
                     SCS_ATTRIBUTE_TEXCOORD : 3,
                     SCS_ATTRIBUTE_NORMAL : 4};

//shader
var gVertexShaderObject;
var gFragmentShaderObject;
var gShaderProgramObject;
//vaos
//shader binding objects
var vao;

//vbos
var vbo;
var vbo_texture;
var smiley_texture;

//matrices
var mvpMatrixUniform;
var textureSamplerUnifirm;
var perspecttiveProjectionMatrix;

var requestAnimationFrame = window.requestAnimationFrame ||
                          window.webkitAnimationRequestFrame ||
                          window.mozRequestAnimationFrame ||
                          window.oRequestAnimationFrame ||
                          window.msRequestAnimationFrame;

function main(){
      // step 1 : get canvas from DOM
      // var : auto type inference JS decieds the type by right side elemnt assigned to variable
      // document : inbuild object of dom
      canvas = document.getElementById("scs");
      if(!canvas){
          // console : inbuild object of dom
          console.log("Obtaining canvas failed");
      }
      else{
          console.log("Obtaining canvas succeded");
      }
      //step 2 : get width and height of canvas for sake of information
      canvas_original_width  = canvas.width;
      canvas_original_height = canvas.height;

      //context.fillStyle = "black";
    //gl.fillRect(0,0,canvas.width,canvas.height); //params : x, y, width , height

    //window Inbuild variable
    //window is inharited from document , its part of document tree
    window.addEventListener("keydown",keyDown,false);
    //3rd parameter true : captured and false: bubble propagtion of event
    window.addEventListener("click",mouseDown,false);
    window.addEventListener("resize",resize,false);

    init();
    resize(); //till now in windows,linuz and android we give warm up resize call for repaint
    draw(); //here we are giving warmup call for draw too for epaint, previously we used to do int in game loop/event loop
}

//change 1
function toggleFullscreen(){
    var fullscreen_element = document.fullscreenElement ||          //chrome,opera and all remaining browers except bellow mentioned
                              document.webkitFullscreenElement ||   //apple suffari
                              document.mozFullScreenElement ||      //mozzila fiewfox
                              document.msFullscreenElement ||       //internet explorer
                              null;                                 //if nothing applies

    if(fullscreen_element == null){
        if(canvas.requestFullscreen){
          canvas.requestFullscreen();
        }
        else if(canvas.webkitRequestFullscreen){
          canvas.webkitRequestFullscreen();
        }
        else if(canvas.mozRequestFullScreen){
          canvas.mozRequestFullScreen();
        }
        else if(canvas.msRequestFullscreen){
          canvas.msRequestFullscreen();
        }
        bFullScreen = true;
    }
    else{
        if(document.exitFullscreen){
          //chrome,opera
          document.exitFullscreen();
        }
        else if(document.webkitExitFullscreen){
          //saffari browser
          document.webkitExitFullscreen();
        }
        else if(document.mozCancleFullScreen){
          //mozilla
          document.mozCancleFullScreen();
        }
        else if(document.msExitFullscreen){
          //internet explorer
          document.msExitFullscreen();
        }
        bFullScreen = false;
    }
}

function init(){
  //to maintain code consistancy to initialize gl context in initial
  gl = canvas.getContext("webgl2");
  if(!gl){
      console.log("Obtaining webgl2 Context failed");
  }
  else{
      console.log("Obtaining webgl2 Context succeded");
  }
  gl.viewportWidth = canvas.width;
  gl.viewportHeight = canvas.height;

//vertex shader
  var vertexShaderSourcecode = (
  				"#version 300 es" +
  				"\n"+
  				"in vec4 vPosition;"+
          "in vec2 vTexcord;"+
  				"uniform mat4 u_mvpMatrix;"+
          "out vec2 outTexcord;"+
  				"void main(void)"+
  				"{"+
  					"gl_Position = u_mvpMatrix * vPosition;"+
            "outTexcord = vTexcord;"+
  				"}");
  gVertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
  gl.shaderSource(gVertexShaderObject,vertexShaderSourcecode);
  gl.compileShader(gVertexShaderObject);
  if(gl.getShaderParameter(gVertexShaderObject,gl.COMPILE_STATUS)==false){
      var error = gl.getShaderInfoLog(gVertexShaderObject);
      if(error.length > 0){
          alert(error);
          uninitialize();
      }
  }

//fragment shader
  var fragmentShaderSourceCode =
		"#version 300 es "+
		"\n"+
    "precision highp float;"+
    "in vec2 outTexcord;"+
    "uniform highp sampler2D u_texture_sampler;"+
		"out vec4 FragColor;"+
		"void main(void)"+
		"{"+
    "	FragColor = texture(u_texture_sampler,outTexcord);"+
		"}";
  gFragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
  gl.shaderSource(gFragmentShaderObject,fragmentShaderSourceCode);
  gl.compileShader(gFragmentShaderObject);
  if(gl.getShaderParameter(gFragmentShaderObject,gl.COMPILE_STATUS)==false){
      var error = gl.getShaderInfoLog(gFragmentShaderObject);
      if(error.length > 0){
          alert(error);
          uninitialize();
      }
  }

  gShaderProgramObject = gl.createProgram();
  gl.attachShader(gShaderProgramObject,gVertexShaderObject);
  gl.attachShader(gShaderProgramObject,gFragmentShaderObject);

  gl.bindAttribLocation(gShaderProgramObject,webGLMacros.SCS_ATTRIBUTE_POSITION,"vPosition");
  gl.bindAttribLocation(gShaderProgramObject, webGLMacros.SCS_ATTRIBUTE_TEXCOORD, "vTexcord");

  gl.linkProgram(gShaderProgramObject);
  if(!gl.getProgramParameter(gShaderProgramObject,gl.LINK_STATUS)){
    var error = gl.getProgramInfoLog(gShaderProgramObject);
    if(error.length > 0){
        alert(error);
        uninitialize();
    }
  }

  mvpMatrixUniform = gl.getUniformLocation(gShaderProgramObject, "u_mvpMatrix");
  textureSamplerUnifirm = gl.getUniformLocation(gShaderProgramObject, "u_texture_sampler"); ///verify once

  var squareVertices = new Float32Array([
    -1.0,1.0,0.0,
    -1.0,-1.0,0.0,
    1.0,-1.0,0.0,
    1.0,1.0,0.0
  ]);

  var textureVertices = new Float32Array([
    0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0
  ]);
  vao = gl.createVertexArray();
  gl.bindVertexArray(vao);

  vbo = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER,vbo);
  gl.bufferData(gl.ARRAY_BUFFER,squareVertices,gl.STATIC_DRAW)
  gl.vertexAttribPointer(webGLMacros.SCS_ATTRIBUTE_POSITION,3,gl.FLOAT,false,0,0);
  gl.enableVertexAttribArray(webGLMacros.SCS_ATTRIBUTE_POSITION);
  gl.bindBuffer(gl.ARRAY_BUFFER,null);

  vbo_texture = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER,vbo_texture);
  gl.bufferData(gl.ARRAY_BUFFER,textureVertices,gl.STATIC_DRAW)
  gl.vertexAttribPointer(webGLMacros.SCS_ATTRIBUTE_TEXCOORD,2,gl.FLOAT,false,0,0);
  gl.enableVertexAttribArray(webGLMacros.SCS_ATTRIBUTE_TEXCOORD);
  gl.bindBuffer(gl.ARRAY_BUFFER,null);

  gl.bindVertexArray(null);
  gl.clearColor(0.0,0.0,0.0,1.0);

  smiley_texture = gl.createTexture(); //gentexture
  smiley_texture.image = new Image();
  smiley_texture.image.src = "smiley.png";
  smiley_texture.image.onload = function(){
  console.log(smiley_texture.image.width);
  console.log(smiley_texture.image.height);

      gl.bindTexture(gl.TEXTURE_2D,smiley_texture);
      gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL,1);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
		  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
      gl.texImage2D(gl.TEXTURE_2D,0,gl.RGB,gl.RGB,gl.UNSIGNED_BYTE,smiley_texture.image);
      gl.bindTexture(gl.TEXTURE_2D,null);
  };

  perspecttiveProjectionMatrix = mat4.create();
  // blue color with alpha 1
  //webGL is js api for openGL es to do opengl 2d ,3d grhapic programming
}

function resize(){
  if(bFullScreen == true){
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
  }
  else{
    canvas.width = canvas_original_width;
    canvas.height = canvas_original_height;
  }
  gl.viewport(0,0,canvas.width,canvas.height);

  mat4.perspective(perspecttiveProjectionMatrix,45.0,(parseFloat(canvas.height)/parseFloat(canvas.width)),0.1,100.0);
}

function draw(){
    gl.clear(gl.COLOR_BUFFER_BIT);

    gl.useProgram(gShaderProgramObject);

    var modelViewMAtrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();
    var translationMatrix = mat4.create();
    mat4.translate(modelViewMAtrix,modelViewMAtrix,[0.0,0.0,-4.0]);
    mat4.multiply(modelViewProjectionMatrix,perspecttiveProjectionMatrix,modelViewMAtrix);
    gl.uniformMatrix4fv(mvpMatrixUniform,false,modelViewProjectionMatrix);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, smiley_texture); //
    gl.uniform1i(textureSamplerUnifirm, 0);

    gl.bindVertexArray(vao);
      gl.drawArrays(gl.TRIANGLE_FAN,0,4);
    gl.bindVertexArray(null);

    gl.useProgram(null);

    requestAnimationFrame(draw,canvas) ;// swap/request buffer //recurssion calling a function by
}

function uninitialize(){
  if(vao){
    gl.deleteVertexArrays(vao);
		vao = null;
  }
  if(vbo){
    gl.deleteBuffer(vbo);
    vbo = null;
  }
  if(vbo_texture){
    gl.deleteBuffer(vbo_texture);
    vbo_texture = null;
  }
  if(smiley_texture){
    gl.deleteTexture(smiley_texture);
    smiley_texture = null;
  }
  //safe release changes
	if (gShaderProgramObject) {
	   if(gVertexShaderObject){
        gl.detachShader(gShaderProgramObject,gVertexShaderObject);
        gl.deleteShader(gVertexShaderObject);
        gVertexShaderObject = null;
     }
     if(gFragmentShaderObject){
       gl.detachShader(gShaderProgramObject,gFragmentShaderObject);
       gl.deleteShader(gFragmentShaderObject);
       gFragmentShaderObject = null;
     }
     gl.deleteProgram(gShaderProgramObject);
     gShaderProgramObject = null;
	}

}

//event addEventListener
function keyDown(event){
  //here event is type inference parameter variable
  switch (event.keyCode) {
    case 70 :
        toggleFullscreen();
      break;
  }
}

function mouseDown(event){

}
