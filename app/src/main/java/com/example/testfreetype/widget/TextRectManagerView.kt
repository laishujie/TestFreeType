package com.example.testfreetype.widget

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Paint
import android.graphics.PorterDuff
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View
import com.example.testfreetype.bean.TextInfo
import com.example.testfreetype.bean.TextLayer
import com.example.testfreetype.util.PathHelp
import com.example.testfreetype.util.SizeUtils
import java.io.File

class TextRectManagerView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0
) : View(context, attrs, defStyleAttr) {

    //已经确定的层边框
    private var textRectAll = ArrayList<TextRect>()

    //临时预览层
    private val previewTextRect by lazy {
        val textRect = TextRect(TextLayer(tmpTextInfo))
        textRect.init(width, height)
        textRect
    }

    private val paint by lazy {
        val mPaint = Paint(Paint.ANTI_ALIAS_FLAG)
        mPaint.style = Paint.Style.FILL
        mPaint
    }

    //当前预览的默认内置textInfo
    private val tmpTextInfo by lazy {
        TextInfo(
            0,
            "输入文字",
            PathHelp.getFontsPath(getContext()) + File.separator + "DroidSansFallback.ttf"
        )
    }


    fun onChangeArea(layerId: Int, left: Float, top: Float, right: Float, bottom: Float) {
        previewTextRect.onChangeArea(left, top, right, bottom)
        invalidate()
    }

    fun getDefaultPreviewTextInfo(): TextInfo {
        return tmpTextInfo
    }


    override fun onDraw(canvas: Canvas?) {
        super.onDraw(canvas)

        canvas?.apply {
            //临时层绘制
            previewTextRect.draw()?.let { bitmap ->

                drawBitmap(bitmap, 0f, 0f, paint)

            }
        }
    }

    override fun onTouchEvent(event: MotionEvent?): Boolean {





        return super.onTouchEvent(event)
    }
}




//框的样式，只负责绘制
class TextRect(private var layer: TextLayer?) {
    private var cache: Bitmap? = null
    private val canvas = Canvas()
    private val textPadding by lazy {
        SizeUtils.dp2px(10f)
    }
    private val linePaint by lazy {
        val mPaint = Paint(Paint.ANTI_ALIAS_FLAG)
        mPaint.color = -0x1
        mPaint.style = Paint.Style.STROKE
        mPaint.strokeWidth = SizeUtils.dp2px(1f).toFloat()
        mPaint
    }

    fun init(width: Int, height: Int) {
        cache = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
        canvas.setBitmap(cache)
    }

    fun draw(): Bitmap? {
        layer?.apply {
            if (rect.width() != 0f) {
                canvas.drawColor(0, PorterDuff.Mode.CLEAR)
                canvas.drawRect(
                    rect.left - textPadding,
                    rect.top - textPadding,
                    rect.right + textPadding,
                    rect.bottom + textPadding,
                    linePaint
                )
                canvas.save()
                canvas.rotate(10f,rect.centerX(),rect.centerY())
                canvas.restore()
            }
        }

        return cache
    }

    fun onChangeArea(left: Float, top: Float, right: Float, bottom: Float) {
        layer?.apply {
            rect.left = left
            rect.top = top
            rect.right = right
            rect.bottom = bottom
        }
        draw()
    }


}