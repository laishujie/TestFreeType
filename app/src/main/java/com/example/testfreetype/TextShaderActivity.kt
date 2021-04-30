package com.example.testfreetype

import android.opengl.GLSurfaceView
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.example.testfreetype.databinding.ActivityTextShaderBinding
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class TextShaderActivity : AppCompatActivity() {

    val ttfPath by lazy {
        intent.getStringExtra("path")
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val binding = ActivityTextShaderBinding.inflate(layoutInflater);
        setContentView(binding.root)

        binding.glSurface.setEGLContextClientVersion(3)
        binding.glSurface.setRenderer(object : GLSurfaceView.Renderer {
            override fun onDrawFrame(gl: GL10?) {
                val text = binding.edInset.text.toString()
                if(text.isEmpty()) return
                TextEngineJni.glRenderText(TextEngineJni.TEXT_ENGINE_ID,ttfPath,text)
            }

            override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
                TextEngineJni.glInitTextShader(TextEngineJni.TEXT_ENGINE_ID, width, height)
            }

            override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {

            }
        })

        binding.glSurface.renderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY

        binding.btnInset.setOnClickListener {
            binding.glSurface.requestRender()
        }
    }
}