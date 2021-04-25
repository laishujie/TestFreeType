package com.example.testfreetype

import android.graphics.BitmapFactory
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import com.example.testfreetype.databinding.ActivityTestFontTextureBinding
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10


class TestFontTextureActivity : AppCompatActivity() {

    enum class DRAW_MODEL {
        INIT, DRAW_R
    }

    private var currDraw = DRAW_MODEL.INIT

    private var textEngine = 0L;

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val binding = ActivityTestFontTextureBinding.inflate(layoutInflater)

        setContentView(binding.root)

        binding.glSurface.setEGLContextClientVersion(3)

        val path = intent.getStringExtra("path")

        binding.glSurface.setRenderer(object : GLSurfaceView.Renderer {
            override fun onDrawFrame(gl: GL10?) {
                if (currDraw == DRAW_MODEL.INIT) {
                    TypeJni.testSdfShaderOnDraw(textEngine,"", binding.xSeekBar.progress.toFloat()/binding.xSeekBar.max)
                }
            }

            override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
                textEngine = TypeJni.testSdfShaderInit(path, width, height,null)
            }

            override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {

            }
        })
        binding.glSurface.renderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY


//
        binding.xSeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                binding.glSurface.requestRender()
                binding.xText.text = "x : $progress"
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
            }
        })
//        y_seekBar.setOnSeekBarChangeListener(object :SeekBar.OnSeekBarChangeListener{
//            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
//                currDraw = DRAW_MODEL.DRAW_R
//                look_y= progress.toFloat()
//                gl_surface.requestRender()
//            }
//
//            override fun onStartTrackingTouch(seekBar: SeekBar?) {
//            }
//
//            override fun onStopTrackingTouch(seekBar: SeekBar?) {
//            }
//        })
//        z_seekBar.setOnSeekBarChangeListener(object :SeekBar.OnSeekBarChangeListener{
//            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
//                currDraw = DRAW_MODEL.DRAW_R
//                look_z =progress.toFloat()
//                gl_surface.requestRender()
//            }
//
//            override fun onStartTrackingTouch(seekBar: SeekBar?) {
//            }
//
//            override fun onStopTrackingTouch(seekBar: SeekBar?) {
//            }
//        })
//
//
//        gl_surface.renderMode = RENDERMODE_WHEN_DIRTY

//        gl_surface.setOnTouchListener { v, event ->
//            when(event?.action){
//                MotionEvent.ACTION_DOWN->{
//
//                }
//                MotionEvent.ACTION_MOVE->{
//                    val vertexWithPoint = PointUtils.vertexWithPoint(
//                        event.x,
//                        event.y,
//                        gl_surface.width,
//                        gl_surface.height
//                    )
//                    look_x=vertexWithPoint[0]
//                    look_y=vertexWithPoint[1]
//                    currDraw = DRAW_MODEL.DRAW_R
//                    gl_surface.requestRender()
//                   // ShaderNative.glTestDrawR(vertexWithPoint[0],vertexWithPoint[1],1f)
//                }
//            }
//            false
//        }
    }
}
