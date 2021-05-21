package com.example.testfreetype.widget

import android.content.Context
import android.graphics.*
import android.os.Build
import android.util.AttributeSet
import android.util.Log
import android.view.MotionEvent
import android.view.View
import androidx.annotation.DrawableRes
import androidx.core.content.ContextCompat
import com.example.testfreetype.R
import com.example.testfreetype.bean.TextInfo
import com.example.testfreetype.bean.TextLayer
import com.example.testfreetype.util.PathHelp
import com.example.testfreetype.util.SizeUtils
import java.io.File
import kotlin.math.abs
import kotlin.math.pow


class TextRectManagerView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0
) : View(context, attrs, defStyleAttr) {

    //已经确定的层边框
    private var textRectAll = ArrayList<TextRect>()

    enum class TouchMode {
        NONE, DOWN, NORTH, SOUTH
    }

    private val mDensity by lazy {
        this.resources.displayMetrics.density
    }

    private var mDownX = 0f
    private var mDownY = 0f

    private var currMode = TouchMode.NONE

    //当前点击的matrix
    private val mDownMatrix = Matrix()
    private val mMoveMatrix = Matrix()

    private var mCurrTextRect: TextRect? = null


    //临时预览层
    private val previewTextRect by lazy {
        val textRect = TextRect(TextLayer(tmpTextInfo), this.context)
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
            //选中的框绘制
            mCurrTextRect?.apply {
                draw()?.let { bitmap ->
                    drawBitmap(bitmap, 0f, 0f, paint)
                }
            }
        }
    }

    override fun onTouchEvent(event: MotionEvent?): Boolean {
        event?.apply {
            when (actionMasked) {
                MotionEvent.ACTION_DOWN -> {
                    onTouchDown(this)
                }

                MotionEvent.ACTION_MOVE -> {
                    onTouchMove(this)
                    invalidate()
                }
                MotionEvent.ACTION_POINTER_DOWN -> {

                }
                MotionEvent.ACTION_POINTER_UP -> {
                }
                MotionEvent.ACTION_UP, MotionEvent.ACTION_CANCEL -> {
                    onTouchUp(this)
                }

            }
        }
        return mCurrTextRect != null
    }

    private fun onTouchUp(motionEvent: MotionEvent) {


    }

    private fun onTouchDown(event: MotionEvent): Boolean {
        this.currMode = TouchMode.DOWN
        mDownX = event.x
        mDownY = event.y

        mCurrTextRect = findHandlingTextRect(event)

        mCurrTextRect?.let {
            val findHandlingTextRectButton = findHandlingTextRectButton(event)
            Log.e("11111", "findHandlingTextRectButton $findHandlingTextRectButton")
            mDownMatrix.set(it.getMatrix())
        }
        invalidate()
        return mCurrTextRect != null
    }

    private fun onTouchMove(event: MotionEvent) {
        when (currMode) {
            TouchMode.DOWN -> {
                mCurrTextRect?.let {
                    if ((abs(event.x - this.mDownX) > this.mDensity * 1.0F || abs(event.y - this.mDownY) > this.mDensity * 1.0F)) {
                        this.mMoveMatrix.set(this.mDownMatrix)
                        this.mMoveMatrix.postTranslate(
                            event.x - this.mDownX,
                            event.y - this.mDownY
                        )
                        this.previewTextRect.setMatrix(this.mMoveMatrix)
                    }
                }

            }
        }
    }


    private fun findHandlingTextRect(event: MotionEvent): TextRect? {
        val contain = previewTextRect.contain(event.x, event.y)
        if (contain) {
            return previewTextRect
        }
        return null
    }

    private fun findHandlingTextRectButton(event: MotionEvent): Boolean {
        mCurrTextRect?.apply {
            return scaleButton.contain(event.x, event.y, getMatrix())
        }
        return false
    }
}


//框的样式，只负责绘制
class TextRect(private var layer: TextLayer?, context: Context) {
    private var cache: Bitmap? = null
    private val canvas = Canvas()

    private val matrix: Matrix = Matrix()

    val scaleButton = ButtonMark(context, R.drawable.overlay_transform_selector)

    fun getMatrix(): Matrix {
        return matrix
    }

    private val textPadding by lazy {
        SizeUtils.dp2px(10f)
    }

    private val paddingRect = RectF()

    private val totalBorderRect = RectF()

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
                setPaddingRect(this)
                canvas.setMatrix(matrix)
                canvas.drawColor(0, PorterDuff.Mode.CLEAR)
                canvas.drawRect(
                    paddingRect, linePaint
                )
                scaleButton.draw(canvas, linePaint)
            }
        }
        return cache
    }

    fun contain(x: Float, y: Float): Boolean {
        layer?.apply {
            setPaddingRect(this)
            setTotalBorderRect()
            matrix.mapRect(totalBorderRect)
            return totalBorderRect.contains(x, y)
        }
        return false
    }

    private fun setTotalBorderRect() {
        totalBorderRect.set(paddingRect)
        val buttonRadius = scaleButton.getButtonRadius()
        totalBorderRect.left -= buttonRadius
        totalBorderRect.right += buttonRadius
        totalBorderRect.top -= buttonRadius
        totalBorderRect.bottom += buttonRadius
    }

    private fun setPaddingRect(layer: TextLayer?) {
        layer?.apply {
            paddingRect.set(
                rect.left - textPadding,
                rect.top - textPadding,
                rect.right + textPadding,
                rect.bottom + textPadding
            )

            scaleButton.updateRect(paddingRect)
        }
    }

    fun onChangeArea(left: Float, top: Float, right: Float, bottom: Float) {
        layer?.apply {
            rect.left = left
            rect.top = top
            rect.right = right
            rect.bottom = bottom
        }
        setPaddingRect(layer)
        draw()
    }

    fun setMatrix(mMoveMatrix: Matrix) {
        matrix.set(mMoveMatrix)
    }

}


class ButtonMark(context: Context, @DrawableRes iconRes: Int) {
    private val rectRightBottomRect = RectF()
    private val tmpBottomRect = RectF()

    private val rightBottom by lazy {
        getBitmap(context, iconRes)
    }

    fun getButtonRadius(): Float {
        return rightBottom.width / 2f
    }

    private fun getBitmap(context: Context, @DrawableRes vectorDrawableId: Int): Bitmap {
        val bitmap: Bitmap
        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP) {
            val vectorDrawable = ContextCompat.getDrawable(context, vectorDrawableId)
            bitmap = Bitmap.createBitmap(
                vectorDrawable!!.intrinsicWidth,
                vectorDrawable.intrinsicHeight, Bitmap.Config.ARGB_8888
            )
            val canvas = Canvas(bitmap)
            vectorDrawable.setBounds(0, 0, canvas.width, canvas.height)
            vectorDrawable.draw(canvas)
        } else {
            bitmap = BitmapFactory.decodeResource(context.resources, vectorDrawableId)
        }
        return bitmap
    }


    fun updateRect(paddingRect: RectF) {
        rectRightBottomRect.set(
            paddingRect.right,
            paddingRect.bottom,
            paddingRect.right + rightBottom.width.toFloat(),
            paddingRect.bottom + rightBottom.height.toFloat()
        )
        rectRightBottomRect.offset(-getButtonRadius(), -getButtonRadius())
    }

    fun draw(canvas: Canvas, paint: Paint) {
        canvas.drawRect(
            rectRightBottomRect, paint
        )
        //底部缩放icon
        canvas.drawBitmap(
            rightBottom,
            rectRightBottomRect.left,
            rectRightBottomRect.top,
            null
        )
    }

    fun contain(downX: Float, downY: Float, matrix: Matrix): Boolean {
        tmpBottomRect.set(rectRightBottomRect)
        matrix.mapRect(tmpBottomRect)
        return tmpBottomRect.contains(downX,downY)
    }
}