package com.example.testfreetype

import android.graphics.Color
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import androidx.core.math.MathUtils.clamp
import com.example.testfreetype.databinding.ActivityTestFontTextureBinding
import com.example.testfreetype.widget.ColorPickerView
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10


class TestFontTextureActivity : AppCompatActivity() {

    private var textEngine = 0L

    var stokeColor = Color.BLUE
    var shadowColor = Color.BLUE

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val binding = ActivityTestFontTextureBinding.inflate(layoutInflater)

        setContentView(binding.root)
        binding.glSurface.setEGLContextClientVersion(3)

        stokeColor = binding.colorSeekBar.color

        shadowColor = binding.shadowColorSeekBar.color

        binding.colorText.text ="颜色：${binding.colorSeekBar.colorString}"
        binding.shadowColorText.text ="颜色：${binding.shadowColorSeekBar.colorString}"

        val path = intent.getStringExtra("path")

        binding.glSurface.setRenderer(object : GLSurfaceView.Renderer {
            override fun onDrawFrame(gl: GL10?) {
                  val outLine =0.5f - (binding.ySeekBar.progress.toFloat()/binding.ySeekBar.max)*0.5f

                    TypeJni.testSdfShaderOnDraw(
                        textEngine,
                        stokeColor,
                        shadowColor,
                        binding.xSeekBar.progress.toFloat()/binding.xSeekBar.max,
                        clamp(outLine,0.001f,1f),
                        binding.zSeekBar.progress.toFloat(),
                        binding.angleSeekBar.progress,
                        1f-binding.alphaSeekBar.progress.toFloat()/binding.alphaSeekBar.max
                        )
            }

            override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
                textEngine = TypeJni.testSdfShaderInit(path, width, height,null)
            }

            override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {

            }
        })

        binding.colorSeekBar.setOnColorPickerChangeListener(object : ColorPickerView.OnColorPickerChangeListener {
            override fun onStartTrackingTouch(picker: ColorPickerView?) {
            }

            override fun onColorChanged(picker: ColorPickerView?, color: Int) {
                stokeColor = color
                binding.colorText.text ="颜色：${binding.colorSeekBar.colorString}"
                binding.glSurface.requestRender()

            }

            override fun onStopTrackingTouch(picker: ColorPickerView?) {

            }

        })

        binding.shadowColorSeekBar.setOnColorPickerChangeListener(object : ColorPickerView.OnColorPickerChangeListener {
            override fun onStartTrackingTouch(picker: ColorPickerView?) {
            }

            override fun onColorChanged(picker: ColorPickerView?, color: Int) {
                shadowColor = color
                binding.shadowColorText.text ="颜色：${binding.shadowColorSeekBar.colorString}"
                binding.glSurface.requestRender()
            }

            override fun onStopTrackingTouch(picker: ColorPickerView?) {

            }

        })
        binding.glSurface.renderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY


//
        binding.xSeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                binding.glSurface.requestRender()
                binding.xText.text = "描边阈值：$progress"
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
            }
        })
        binding.ySeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                binding.glSurface.requestRender()
                binding.yText.text = "描边宽度：$progress"
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
            }
        })

        binding.zSeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                binding.glSurface.requestRender()
                binding.zText.text = "距离：$progress"
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
            }
        })

        binding.angleSeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                binding.glSurface.requestRender()
                binding.angleText.text = "角度：$progress"
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
            }
        })

        binding.alphaSeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                binding.glSurface.requestRender()
                binding.alphaText.text = "模糊度：$progress"
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
