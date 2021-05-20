package com.example.testfreetype

import android.graphics.Typeface
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import by.kirich1409.viewbindingdelegate.viewBinding
import com.example.testfreetype.databinding.ActivityTextEditBinding
import com.example.testfreetype.util.*
import com.example.testfreetype.widget.TemplateFragment
import com.example.testfreetype.widget.TextStyleFragment
import java.io.File


class TextEditActivity : AppCompatActivity(R.layout.activity_text_edit) {


    private val editSurfaceManager = TextEditSurfaceManager()

    //当前预览的textInfo
    private val preViewTextInfo by lazy {
        //获取内置的普通文本预览层
        viewBinding.textRectManager.getDefaultPreviewTextInfo()
    }

    val ttfPath by lazy {
        intent.getStringExtra("path").toString()
    }

    val fontPath by lazy {
        PathHelp.getFontsPath(this)
    }

    val jsonPath by lazy {
        intent.getStringExtra("jsonPath").toString()
    }


    private val templateFragment by lazy {
        var fragment = FragmentHelp.restoreFragment(
            this,
            TemplateFragment::class.simpleName
        ) as? TemplateFragment
        if (fragment == null) {
            fragment = TemplateFragment()
            fragment?.selectCallBack = { path ->
                editSurfaceManager.drawPreViewLayerByJson(path, fontPath)
            }
            fragment?.selectOk = {
                editSurfaceManager.addThePreviewLayerByJson2Map()
            }
            FragmentHelp.initFragment(
                this,
                fragment!!,
                R.id.frame_menu,
                TemplateFragment::class.simpleName,
                false
            )
        }
        fragment!!
    }


    private val textStyleBottomSheetFragment by lazy {
        var fragment = FragmentHelp.restoreFragment(
            this,
            TextStyleFragment::class.simpleName
        ) as? TextStyleFragment
        if (fragment == null) {
            fragment = TextStyleFragment()
            fragment?.styleCallBack = textStyleCallBack
            FragmentHelp.initFragment(
                this,
                fragment!!,
                R.id.frame_menu,
                TextStyleFragment::class.simpleName,
                false
            )
        }
        fragment!!
    }

    private val textStyleCallBack = object : TextStyleFragment.OnStyleChange {
        override fun changeText(text: String) {
            editSurfaceManager
            preViewTextInfo.apply {
                char = text
                editSurfaceManager.drawPreViewLayer(this)
            }
        }

        override fun changeHorizontal(horizontal: Boolean) {
            preViewTextInfo.apply {
                this.horizontal = horizontal
                editSurfaceManager.drawPreViewLayer(this)
            }
        }

        override fun changeWordSpacing(spacing: Int) {
            preViewTextInfo.apply {
                this.spacing = spacing
                editSurfaceManager.drawPreViewLayer(this)
            }
        }

        override fun changeLineWordSpacing(spacing: Int) {
            preViewTextInfo.apply {
                this.lineSpacing = spacing
                editSurfaceManager.drawPreViewLayer(this)
            }
        }

        override fun changeFont(fontPath: String) {
            preViewTextInfo.apply {
                this.ttfPath = fontPath
                editSurfaceManager.drawPreViewLayer(this)
            }
        }

        override fun changeFontSize(pixie: Int) {
            preViewTextInfo.apply {
                this.size = if (pixie == 0) 1 else pixie
                editSurfaceManager.drawPreViewLayer(this)
            }
        }

        override fun changeFontColor(color: Int) {
            preViewTextInfo.apply {
                this.fontColor = color
                editSurfaceManager.drawPreViewLayer(this)
            }
        }

        override fun changeDistance(distanceMark: Float) {
            preViewTextInfo.apply {
                this.distanceMark = distanceMark
                editSurfaceManager.drawPreViewLayer(this)
            }
        }

        override fun changeOutlineDistanceMark(outLineDistanceMark: Float) {
            preViewTextInfo.apply {
                this.outLineDistanceMark = outLineDistanceMark
                editSurfaceManager.drawPreViewLayer(this)
            }
        }

        override fun changeOutlineColor(color: Int) {
            preViewTextInfo.apply {
                this.outLineColor = color
                editSurfaceManager.drawPreViewLayer(this)
            }
        }

        override fun changeShadowDistance(shadowDistance: Float) {
            preViewTextInfo.apply {
                this.shadowDistance = shadowDistance
                editSurfaceManager.drawPreViewLayer(this)
            }
        }

        override fun changeShadowAlpha(shadowAlpha: Float) {
            preViewTextInfo.apply {
                this.shadowAlpha = shadowAlpha
                editSurfaceManager.drawPreViewLayer(this)
            }
        }

        override fun changeShadowColor(shadowColor: Int) {
            preViewTextInfo.apply {
                this.shadowColor = shadowColor
                editSurfaceManager.drawPreViewLayer(this)
            }
        }

        override fun changeShadowAngle(shadowAngle: Int) {
            preViewTextInfo.apply {
                this.shadowAngle = shadowAngle
                editSurfaceManager.drawPreViewLayer(this)
            }
        }

        override fun onCreateLayer() {
            //TODO 这里应该需要重新复制临时配置
            preViewTextInfo.char = null
            editSurfaceManager.addThePreviewLayer2Map()

        }
    }

    private val viewBinding by viewBinding(ActivityTextEditBinding::bind)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)


        //AndroidBug5497Workaround.assistActivity(this)

        viewBinding.surfaceView.surfaceTextureListener = editSurfaceManager
        val typeface = Typeface.createFromFile(
            File(
                StorageHelper.getInternalFilesDir(this),
                "fonts/DroidSansFallback.ttf"
            )
        )

        viewBinding.tvTest.typeface = typeface
        viewBinding.tvTest.text = "Test"

        viewBinding.btnText.setOnClickListener {
            /*editSurfaceManager.addTextLayer(
                TextLayer(
                    TextInfo(-1, "你好呀", ttfPath)
                )
            )*/
            FragmentHelp.showOrHideFragment(this, textStyleBottomSheetFragment)
        }

        viewBinding.tvTemplate.setOnClickListener {
            FragmentHelp.showOrHideFragment(this, templateFragment)
        }

        //注册层次移动回调
        TextEngineHelper.getTextEngine()
            .registerTextAreaCallBack { layerId, left, top, right, bottom ->
                viewBinding.textRectManager.onChangeArea(layerId, left, top, right, bottom)
            }
    }


}