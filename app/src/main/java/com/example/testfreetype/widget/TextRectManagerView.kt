package com.example.testfreetype.widget

import android.content.Context
import android.graphics.*
import android.os.Build
import android.util.AttributeSet
import android.util.Log
import android.util.SparseArray
import android.view.MotionEvent
import android.view.View
import androidx.annotation.DrawableRes
import androidx.core.content.ContextCompat
import androidx.core.util.forEach
import com.example.testfreetype.R
import com.example.testfreetype.bean.MatrixInfo
import com.example.testfreetype.bean.TextLayer
import com.example.testfreetype.util.SizeUtils

class TextRectManagerView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0
) : View(context, attrs, defStyleAttr) {

    //已经确定的层边框
    private var textRectAll = SparseArray<TextRect>()

    fun printAll() {
        textRectAll.forEach { key, value ->
            value.getLayer()?.apply {
                textList.forEach {
                    Log.e(
                        "11111",
                        "layerId $key subId ${it.id} char ${it.char}"
                    )
                }
            }
        }
    }

    private var currMatrixInfo = MatrixInfo(0f, 0f, 1f, 0f)

    var ITransformCallback: ((Int, Float, Float, Float, Float) -> Unit?)? = null

    var ISelectLayerCallBack: ((TextLayer?) -> Unit)? = null


    enum class TouchMode {
        NONE, DOWN, ICON
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
    private var mCurrButtonMark: BaseButtonMark? = null

    private var viewWidth = 0
    private var viewHeight = 0

    fun getMatrixInfo(): MatrixInfo {
        mCurrTextRect?.apply {
            val v = FloatArray(9)
            getMatrix().getValues(v)
            val tx = v[Matrix.MTRANS_X]
            val ty = v[Matrix.MTRANS_Y]
            val scalex = v[Matrix.MSCALE_X]
            val skewy = v[Matrix.MSKEW_Y]
            val rScale =
                Math.sqrt(scalex * scalex + skewy * skewy.toDouble()).toFloat()

            val rAngle = Math.round(
                Math.atan2(
                    v[Matrix.MSKEW_X].toDouble(),
                    v[Matrix.MSCALE_X].toDouble()
                ) * (180 / Math.PI)
            ).toFloat()
            currMatrixInfo.rangle = rAngle


            currMatrixInfo.tx = tx
            currMatrixInfo.ty = ty
            currMatrixInfo.scale = rScale
        }
        return currMatrixInfo
    }

    private val paint by lazy {
        val mPaint = Paint(Paint.ANTI_ALIAS_FLAG)
        mPaint.style = Paint.Style.FILL
        mPaint
    }

    fun addRect(textLayer: TextLayer) {
        val textRect = TextRect(textLayer, context)
        textRect.init(viewWidth, viewHeight)
        textRectAll.put(textLayer.layerId, textRect)
    }


    fun removeRect(layerId: Int) {
       textRectAll.remove(layerId)
        notifity(layerId)
    }

    fun notifity(layerId: Int) {
        mCurrTextRect = textRectAll.get(layerId)
        invalidate()
    }

    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) {
        super.onSizeChanged(w, h, oldw, oldh)
        viewWidth = measuredWidth
        viewHeight = measuredHeight
    }

    fun onChangeArea(layerId: Int, left: Float, top: Float, right: Float, bottom: Float) {
        mCurrTextRect = textRectAll[layerId]
        mCurrTextRect?.onChangeArea(left, top, right, bottom)
        invalidate()
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

    private fun updateMatrix() {
        getMatrixInfo().apply {
            mCurrTextRect?.let {
                val rect = it.getRect()
                ITransformCallback?.invoke(
                    it.getLayerId()!!,
                    rect.centerX(),
                    rect.centerY(),
                    scale,
                    rangle
                )
                Log.e("11111", "rect $rect")
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
        mCurrTextRect = null
        textRectAll.forEach { _, value ->
            if (value.contain(event.x, event.y)) {
                mCurrTextRect = value
                ISelectLayerCallBack?.invoke(mCurrTextRect?.getLayer())
            }
        }

        mCurrTextRect?.let {
            val findHandlingTextRectButton = findHandlingTextRectButton(event)
            if (findHandlingTextRectButton != null) {
                currMode = TouchMode.ICON
                mCurrButtonMark = findHandlingTextRectButton
                mCurrButtonMark?.onDown(event)
                Log.e("11111", "选中icon")
            }
            mDownMatrix.set(it.getMatrix())
        }
        invalidate()
        return mCurrTextRect != null
    }

    private fun onTouchMove(event: MotionEvent) {
        when (currMode) {
            TouchMode.DOWN -> {
                mCurrTextRect?.let {
                    if ((kotlin.math.abs(event.x - this.mDownX) > this.mDensity * 1.0F || kotlin.math.abs(
                            event.y - this.mDownY
                        ) > this.mDensity * 1.0F)
                    ) {
                        this.mMoveMatrix.set(this.mDownMatrix)
                        val dx = event.x - this.mDownX
                        val dy = event.y - this.mDownY
                        this.mMoveMatrix.postTranslate(
                            dx,
                            dy
                        )
                        currMatrixInfo.tx = event.x - this.mDownX
                        currMatrixInfo.ty = event.y - this.mDownY

                        mCurrTextRect?.setMatrix(this.mMoveMatrix)
                        invalidate()
                        updateMatrix()
                    }
                }
            }
            TouchMode.ICON -> {
                mCurrButtonMark?.onMove(mCurrTextRect, mDownMatrix, event, this)
                updateMatrix()
            }
            else -> {

            }
        }
    }


    private fun findHandlingTextRect(event: MotionEvent): TextRect? {
        val contain = mCurrTextRect?.contain(event.x, event.y)
        if (contain != null && contain) {
            Log.e("11111", "选中框")
            return mCurrTextRect
        }
        return null
    }

    private fun findHandlingTextRectButton(event: MotionEvent): BaseButtonMark? {
        mCurrTextRect?.apply {
            if (scaleButton.contain(getMatrix(), event.x, event.y)) {
                return scaleButton
            }
        }
        return null
    }
}


//框的样式，只负责绘制
class TextRect(private var layer: TextLayer?, context: Context) {
    private var cache: Bitmap? = null
    private val canvas = Canvas()

    private val matrix: Matrix = Matrix()
    var width = 0
    var height = 0
    val scaleButton: BaseButtonMark = ButtonMark(context, R.drawable.overlay_transform_selector)

    fun getMatrix(): Matrix {
        return matrix
    }

    private val textPadding by lazy {
        SizeUtils.dp2px(10f)
    }

    private val paddingRect = RectF()

    private val totalBorderRect = RectF()

    private val tmpRect = RectF()
    var mInverseMatrix: Matrix = Matrix()

    fun getLayerId(): Int? {
        return layer?.layerId
    }

    fun getLayer(): TextLayer? {
        return layer
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
        this.width = width
        this.height = height
        canvas.setBitmap(cache)
    }

    fun draw(): Bitmap? {
        layer?.apply {
            if (rect.width() != 0f) {
                canvas.save()
                canvas.setMatrix(matrix)
                setPaddingRect(this)
                linePaint.color = Color.WHITE

                canvas.drawColor(0, PorterDuff.Mode.CLEAR)

                canvas.drawRect(
                    paddingRect, linePaint
                )

                linePaint.color = Color.GRAY
                canvas.drawRect(
                    totalBorderRect, linePaint
                )

                scaleButton.draw(canvas, matrix, linePaint)
                canvas.restore()
            }
            linePaint.color = Color.RED

            canvas.drawRect(
                getRect(), linePaint
            )
        }
        return cache
    }

    fun contain(x: Float, y: Float): Boolean {
        layer?.apply {
            setPaddingRect(this)
            setTotalBorderRect()
            val dst = FloatArray(2)
            mInverseMatrix.reset()
            matrix.invert(mInverseMatrix)
            mInverseMatrix.mapPoints(dst, floatArrayOf(x, y))

            return totalBorderRect.contains(dst[0], dst[1])
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
            scaleButton.updateRect(paddingRect, matrix)
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

    fun getRect(): RectF {
        tmpRect.set(totalBorderRect)
        matrix.mapRect(tmpRect)
        return tmpRect
    }

    fun setMatrix(mMoveMatrix: Matrix) {
        matrix.set(mMoveMatrix)
    }

}

abstract class BaseButtonMark(context: Context, @DrawableRes iconRes: Int) {

    protected val iconRect = RectF()
    protected var storkRect = RectF()

    protected val iconBitmap by lazy {
        getBitmap(context, iconRes)
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

    fun getButtonRadius(): Float {
        return iconBitmap.width / 2f
    }

    abstract fun updateRect(paddingRect: RectF, matrix: Matrix)


    fun draw(canvas: Canvas, matrix: Matrix, paint: Paint) {
        canvas.drawRect(
            iconRect, paint
        )
        //底部缩放icon
        canvas.drawBitmap(
            iconBitmap,
            iconRect.left,
            iconRect.top,
            null
        )
        //canvas.restore()
    }

    var mInverseMatrix: Matrix = Matrix()

    fun contain(matrix: Matrix, downX: Float, downY: Float): Boolean {
        val dst = FloatArray(2)
        mInverseMatrix.reset()
        matrix.invert(mInverseMatrix)
        mInverseMatrix.mapPoints(dst, floatArrayOf(downX, downY))

        return iconRect.contains(dst[0], dst[1])
    }


    abstract fun onDown(event: MotionEvent)

    abstract fun onMove(
        rectF: TextRect?,
        matrix: Matrix,
        event: MotionEvent,
        view: TextRectManagerView
    )
}

open class ButtonMark(context: Context, @DrawableRes iconRes: Int) :
    BaseButtonMark(context, iconRes) {

    private var previousLocationX = 0f
    private var previousLocationY = 0f

    private var mMoveX = 0f
    private var mMoveY = 0f
    private var oldDegrees = 0f
    private var oldDistance = 0f

    //位置放哪里
    override fun updateRect(paddingRect: RectF, matrix: Matrix) {
        storkRect.set(paddingRect)

        iconRect.set(
            storkRect.right,
            storkRect.bottom,
            storkRect.right + iconBitmap.width.toFloat(),
            storkRect.bottom + iconBitmap.height.toFloat()
        )
        iconRect.offset(-getButtonRadius(), -getButtonRadius())
    }


    override fun onDown(event: MotionEvent) {
        previousLocationX = event.x
        previousLocationY = event.y
    }

    override fun onMove(
        rectF: TextRect?,
        matrix: Matrix,
        event: MotionEvent,
        view: TextRectManagerView
    ) {
        rectF?.apply {
            val rect = getRect()
            Log.e("111111","rect.left"+rect.left)
            val newMoveX = event.x.toInt()
            val newMoveY = event.y.toInt()

            val degrees = calculateRotation(
                newMoveX.toFloat(),
                newMoveY.toFloat(),
                rect.centerX().toInt(),
                rect.centerY().toInt()
            )

            val distance = calculateDistance(
                newMoveX.toFloat(),
                newMoveY.toFloat(),
                rect.centerX().toInt(),
                rect.centerY().toInt()
            )

            if (mMoveX != 0f && mMoveY != 0f) {
                /* matrix.postTranslate(
                     -rect.centerX(),
                     -rect.centerY()
                 )*/// 左乘平移变换矩阵，坐标原点平移到中心点

                matrix.postRotate(
                    degrees - oldDegrees, rect.centerX(),
                    rect.centerY()
                )
                matrix.postScale(
                    distance / oldDistance,
                    distance / oldDistance, rect.centerX(),
                    rect.centerY()
                )

                // matrix.postTranslate(rect.centerX(), rect.centerY())
                setMatrix(matrix)
                view.invalidate()
            }
            oldDegrees = degrees
            oldDistance = distance

            mMoveX = newMoveX.toFloat()
            mMoveY = newMoveY.toFloat()


            previousLocationX = event.x
            previousLocationY = event.y
        }
    }


    private fun calculateRotation(
        x1: Float,
        y1: Float,
        x2: Int,
        y2: Int
    ): Float {
        val x = x1 - x2.toDouble()
        val y = y1 - y2.toDouble()
        val radians = Math.atan2(y, x)
        return Math.toDegrees(radians).toFloat()
    }

    private fun calculateDistance(
        x1: Float,
        y1: Float,
        x2: Int,
        y2: Int
    ): Float {
        val x = x1 - x2.toDouble()
        val y = y1 - y2.toDouble()
        return Math.sqrt(x * x + y * y).toFloat()
    }

}