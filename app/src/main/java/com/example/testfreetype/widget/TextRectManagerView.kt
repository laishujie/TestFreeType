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
import com.example.testfreetype.bean.MatrixInfo
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

    private var currMatrixInfo = MatrixInfo(0f, 0f, 1f, 0f)


    enum class TouchMode {
        NONE, DOWN, ICON, SOUTH
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

    private val first = PointF()
    private val second = PointF()

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
        Log.e("11111","curcurrMatrixInfo $currMatrixInfo")
        return currMatrixInfo
    }

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
        first.set(event.x, event.y)
        second.set(event.x, event.y)
        mCurrTextRect = findHandlingTextRect(event)

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
                        this.mMoveMatrix.postTranslate(
                            event.x - this.mDownX,
                            event.y - this.mDownY
                        )
                        this.previewTextRect.setMatrix(this.mMoveMatrix)
                        invalidate()
                    }
                }
            }
            TouchMode.ICON -> {
                mCurrButtonMark?.onMove(mCurrTextRect, mMoveMatrix, event, this)
            }
            else -> {

            }
        }
    }


    private fun findHandlingTextRect(event: MotionEvent): TextRect? {
        val contain = previewTextRect.contain(event.x, event.y)
        if (contain) {
            Log.e("11111", "选中框")
            return previewTextRect
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
                canvas.save()
                canvas.concat(matrix)
                setPaddingRect(this)

                canvas.drawColor(0, PorterDuff.Mode.CLEAR)

                canvas.drawRect(
                    paddingRect, linePaint
                )

                scaleButton.draw(canvas, matrix, linePaint)
                canvas.restore()
            }
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
    private val mButtonMatrix = Matrix()


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

    var matrixValues = FloatArray(9)

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

    abstract fun onMove(rectF: TextRect?, matrix: Matrix, event: MotionEvent, view: View)

    abstract fun applyMatrix(matrix: Matrix)
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

    override fun onMove(rectF: TextRect?, matrix: Matrix, event: MotionEvent, view: View) {
        rectF?.apply {
            val rect = getRect()
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
                matrix.postRotate(degrees - oldDegrees, rect.centerX(), rect.centerY())
                matrix.postScale(
                    distance / oldDistance,
                    distance / oldDistance,
                    rect.centerX(),
                    rect.centerY()
                )
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


    protected fun calculateRotation(
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

    protected fun calculateDistance(
        x1: Float,
        y1: Float,
        x2: Int,
        y2: Int
    ): Float {
        val x = x1 - x2.toDouble()
        val y = y1 - y2.toDouble()
        return Math.sqrt(x * x + y * y).toFloat()
    }


    fun angle(cen: PointF, first: PointF, second: PointF): Float {
        val dx1: Float = (first.x - cen.x)
        val dy1: Float = (first.y - cen.y)
        val dx2: Float = (second.x - cen.x)
        val dy2: Float = (second.y - cen.y)

        // 计算三边的平方
        val ab2 =
            ((second.x - first.x) * (second.x - first.x) + (second.y - first.y) * (second.y - first.y))
        val oa2 = dx1 * dx1 + dy1 * dy1
        val ob2 = dx2 * dx2 + dy2 * dy2

        // 根据两向量的叉乘来判断顺逆时针
        val isClockwise =
            (first.x - cen.x) * (second.y - cen.y) - (first.y - cen.y) * (second.x - cen.x) > 0

        // 根据余弦定理计算旋转角的余弦值
        var cosDegree =
            (oa2 + ob2 - ab2) / (2 * Math.sqrt(oa2.toDouble()) * Math.sqrt(ob2.toDouble()))

        // 异常处理，因为算出来会有误差绝对值可能会超过一，所以需要处理一下
        if (cosDegree > 1) {
            cosDegree = 1.0
        } else if (cosDegree < -1) {
            cosDegree = -1.0
        }

        // 计算弧度
        val radian = Math.acos(cosDegree)

        // 计算旋转过的角度，顺时针为正，逆时针为负
        return (if (isClockwise) Math.toDegrees(radian) else -Math.toDegrees(
            radian
        )).toFloat()
    }


    override fun applyMatrix(matrix: Matrix) {

    }


}