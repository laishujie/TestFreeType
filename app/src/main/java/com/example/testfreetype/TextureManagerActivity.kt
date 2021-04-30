package com.example.testfreetype

import android.opengl.GLSurfaceView
import android.os.Bundle
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.testfreetype.databinding.ActivityTestTextureBinding
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class TextureManagerActivity : AppCompatActivity() {

    var textEngine = TextEngineJni.TEXT_ENGINE_ID

    val ttfPath by lazy {
        intent.getStringExtra("path")
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val binding = ActivityTestTextureBinding.inflate(layoutInflater)

        setContentView(binding.root)
        binding.glSurface.setEGLContextClientVersion(3)

        binding.tvUseInfo.text = "TTF：${ttfPath} \n缓存纹理大小：512*512 \n缓存文字大小： 32 "

        binding.glSurface.setRenderer(object : GLSurfaceView.Renderer {
            override fun onDrawFrame(gl: GL10?) {
                TextEngineJni.freeDraw(textEngine)
            }

            override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
                TextEngineJni.testFreeTypeInit(textEngine, ttfPath, width, height)
            }

            override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {

            }
        })

        binding.glSurface.renderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY

        binding.btnTextInfo.setOnClickListener {
            val text = binding.edText.text.toString()
            if (text.isEmpty()) return@setOnClickListener
            if(text.length>2){
                Toast.makeText(this,"请输入一个字符",Toast.LENGTH_LONG).show()
                return@setOnClickListener
            }
            binding.tvInfo.text = "\"$text\" 偏移信息: \n      "+TextEngineJni.printTextInfo(textEngine, ttfPath, text)
        }


        binding.btnInset.setOnClickListener {
            val text = binding.edInset.text.toString()
            if (text.isEmpty()) return@setOnClickListener;
            binding.glSurface.queueEvent {
                TextEngineJni.insetText(textEngine, ttfPath, text)
                binding.glSurface.requestRender()
            }
        }


    }
}