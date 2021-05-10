package com.example.testfreetype

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.example.testfreetype.bean.TextConfigManager
import com.example.testfreetype.databinding.ActivityTextEditBinding
import com.example.testfreetype.util.SoftInputUtil
import com.example.testfreetype.util.TextEditSurfaceManager
import com.example.testfreetype.widget.TextStyleBottomSheetFragment


class TextEditActivity : AppCompatActivity() {

    private val editSurfaceManager = TextEditSurfaceManager()
    private val softInputUtil = SoftInputUtil()

    val ttfPath by lazy {
        intent.getStringExtra("path").toString()
    }

    private val textStyleBottomSheetFragment by lazy {
        var fragment =
            supportFragmentManager.findFragmentByTag("TextStyleBottomSheetFragment") as? TextStyleBottomSheetFragment
        if (fragment == null) {
            fragment = TextStyleBottomSheetFragment()
        }
        fragment!!
    }

    var addLayerId = 0;

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val inflate = ActivityTextEditBinding.inflate(layoutInflater)
        setContentView(inflate.root)

        inflate.surfaceView.surfaceTextureListener = editSurfaceManager


        inflate.btnText.setOnClickListener {
            textStyleBottomSheetFragment.show(
                supportFragmentManager,
                "TextStyleBottomSheetFragment"
            )
        }

        addLayerId = TextConfigManager.addLayer(ttfPath, "")

        textStyleBottomSheetFragment.styleCallBack =
            object : TextStyleBottomSheetFragment.OnStyleChange {
                override fun changeText(text: String) {
                    TextConfigManager.getLayer(addLayerId)?.apply {
                        char = text
                        editSurfaceManager.drawLayer(this)
                    }
                }

                override fun changeHorizontal(horizontal: Boolean) {
                    TextConfigManager.getLayer(addLayerId)?.apply {
                        this.horizontal = horizontal
                        editSurfaceManager.drawLayer(this)
                    }
                }

                override fun changeWordSpacing(spacing: Int) {
                    TextConfigManager.getLayer(addLayerId)?.apply {
                        this.spacing = spacing
                        editSurfaceManager.drawLayer(this)
                    }
                }

                override fun changeLineWordSpacing(spacing: Int) {
                    TextConfigManager.getLayer(addLayerId)?.apply {
                        this.lineSpacing = spacing
                        editSurfaceManager.drawLayer(this)
                    }
                }

                override fun changeFont(fontPath: String) {
                    TextConfigManager.getLayer(addLayerId)?.apply {
                        this.ttfPath = fontPath
                        editSurfaceManager.drawLayer(this)
                    }
                }

                override fun changeFontSize(pixie: Int) {
                    TextConfigManager.getLayer(addLayerId)?.apply {
                        this.size = if(pixie==0)1 else pixie
                        editSurfaceManager.drawLayer(this)
                    }
                }

                override fun changeFontColor(color: Int) {
                    TextConfigManager.getLayer(addLayerId)?.apply {
                        this.fontColor = color
                        editSurfaceManager.drawLayer(this)
                    }
                }

                override fun changeDistance(distanceMark: Float) {
                    TextConfigManager.getLayer(addLayerId)?.apply {
                        this.distanceMark = distanceMark
                        editSurfaceManager.drawLayer(this)
                    }
                }

                override fun changeOutlineDistanceMark(outLineDistanceMark: Float) {
                    TextConfigManager.getLayer(addLayerId)?.apply {
                        this.outLineDistanceMark = outLineDistanceMark
                        editSurfaceManager.drawLayer(this)
                    }
                }
            }

        /*softInputUtil.attachSoftInput(
            inflate.btnText
        ) { isSoftInputShow, _, viewOffset ->
            if (isSoftInputShow) {
                inflate.btnText.translationY = inflate.btnText.translationY - viewOffset - SizeUtils.dp2px(10f)
                inflate.viewBg.translationY = inflate.btnText.translationY
                //inflate.surfaceView.translationY= inflate.btnText.translationY
            } else {
                inflate.btnText.translationY = 0f
                inflate.viewBg.translationY=0f
                //inflate.surfaceView.translationY= 0f
            }
        }*/
    }
}