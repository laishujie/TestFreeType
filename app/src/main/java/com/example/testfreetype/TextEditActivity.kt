package com.example.testfreetype

import android.graphics.Typeface
import android.os.Bundle
import android.util.Log
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import by.kirich1409.viewbindingdelegate.viewBinding
import com.example.testfreetype.bean.TextLayer
import com.example.testfreetype.bean.TextLayerManager
import com.example.testfreetype.databinding.ActivityTextEditBinding
import com.example.testfreetype.util.*
import com.example.testfreetype.widget.TemplateFragment
import com.example.testfreetype.widget.TextStyleFragment
import java.io.File


class TextEditActivity : AppCompatActivity(R.layout.activity_text_edit) {
    //文本管理器
    private val editSurfaceManager = TextEditSurfaceManager()

    //当前操作的层
    private var tmpTextLayer: TextLayer? = null

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
                val createByJson = TextLayerManager.createByJson(this, File(path))
                TextEngineHelper.getTextEngine().addTextLayer(createByJson)
                tmpTextLayer = createByJson
                tmpTextLayer?.apply {
                    viewBinding.textRectManager.addRect(this)
                }
                null
                // editSurfaceManager.drawPreViewLayerByJson(path, fontPath)
            }

            fragment?.exitOk = {
                tmpTextLayer?.apply {
                    TextEngineHelper.getTextEngine().removeTextLayer(layerId)
                    viewBinding.textRectManager.removeRect(layerId)
                }
                null
            }

            /* */
            /*fragment?.selectOk = {
                editSurfaceManager.addThePreviewLayerByJson2Map()
            }*/
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
        FragmentHelp.restoreFragment(
            this,
            TextStyleFragment::class.simpleName
        ) as TextStyleFragment
    }

    private fun InitTextStyleFragment() {
        var fragment = FragmentHelp.restoreFragment(
            this,
            TextStyleFragment::class.simpleName
        ) as? TextStyleFragment
        if (fragment == null) {
            fragment = TextStyleFragment()
            fragment.styleCallBack = textStyleCallBack
            FragmentHelp.initFragment(
                this,
                fragment,
                R.id.frame_menu,
                TextStyleFragment::class.simpleName,
                false
            )
        }
    }


    private val textStyleCallBack = object : TextStyleFragment.OnStyleChange {

        override fun exit() {
            tmpTextLayer?.apply {
                TextEngineHelper.getTextEngine().removeTextLayer(layerId)
                viewBinding.textRectManager.removeRect(layerId)
            }
        }

        override fun changeText(text: String) {
            tmpTextLayer?.apply {
                getFirst()?.char = text
                editSurfaceManager.setBasicTextAttributes(this)
            }
        }

        override fun changeFont(fontPath: String) {
            tmpTextLayer?.apply {
                getFirst()?.ttfPath = fontPath
                editSurfaceManager.setBasicTextAttributes(this)
            }
        }

        override fun changeFontSize(pixie: Int) {
            tmpTextLayer?.apply {
                getFirst()?.size = pixie
                editSurfaceManager.setBasicTextAttributes(this)
            }
        }

        override fun changeFontColor(color: Int) {
            tmpTextLayer?.apply {
                getFirst()?.fontColor = color
                editSurfaceManager.setBasicTextAttributes(this)
            }
        }


        override fun changeDistance(distanceMark: Float) {
            tmpTextLayer?.apply {
                getFirst()?.distanceMark = distanceMark
                Log.e("11111", "distanceMark $distanceMark")
                editSurfaceManager.setStrokeAttributes(this)
            }
        }

        override fun changeOutlineDistanceMark(outLineDistanceMark: Float) {
            tmpTextLayer?.apply {
                getFirst()?.outLineDistanceMark = outLineDistanceMark
                Log.e("11111", "outLineDistanceMark $outLineDistanceMark")

                editSurfaceManager.setStrokeAttributes(this)
            }
        }

        override fun changeOutlineColor(color: Int) {
            tmpTextLayer?.apply {
                getFirst()?.outLineColor = color
                editSurfaceManager.setStrokeAttributes(this)
            }
        }


        override fun changeHorizontal(horizontal: Boolean) {
            /* preViewTextInfo.apply {
                 this.horizontal = horizontal
                 editSurfaceManager.drawPreViewLayer(this)
             }*/
        }

        override fun changeWordSpacing(spacing: Int) {
            /*  preViewTextInfo.apply {
                  this.spacing = spacing
                  editSurfaceManager.drawPreViewLayer(this)
              }*/
        }

        override fun changeLineWordSpacing(spacing: Int) {
            /*preViewTextInfo.apply {
                this.lineSpacing = spacing
                editSurfaceManager.drawPreViewLayer(this)
            }*/
        }


        override fun changeShadowDistance(shadowDistance: Float) {
            /*preViewTextInfo.apply {
                this.shadowDistance = shadowDistance
                editSurfaceManager.drawPreViewLayer(this)
            }*/
        }

        override fun changeShadowAlpha(shadowAlpha: Float) {
            /* preViewTextInfo.apply {
                 this.shadowAlpha = shadowAlpha
                 editSurfaceManager.drawPreViewLayer(this)
             }*/
        }

        override fun changeShadowColor(shadowColor: Int) {
            /*preViewTextInfo.apply {
                this.shadowColor = shadowColor
                editSurfaceManager.drawPreViewLayer(this)
            }*/
        }

        override fun changeShadowAngle(shadowAngle: Int) {
            /*preViewTextInfo.apply {
                this.shadowAngle = shadowAngle
                editSurfaceManager.drawPreViewLayer(this)
            }*/
        }

        override fun onCreateLayer() {
            //TODO 这里应该需要重新复制临时配置
            //preViewTextInfo.char = null
            // editSurfaceManager.addThePreviewLayer2Map()
        }
    }

    private val viewBinding by viewBinding(ActivityTextEditBinding::bind)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        //AndroidBug5497Workaround.assistActivity(this)
        TextEngineHelper.init()

        InitTextStyleFragment()

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
            //TODO 记得添加Loading
            addSimpleText()
        }

        viewBinding.tvTemplate.setOnClickListener {
            FragmentHelp.showOrHideFragment(this, templateFragment)
        }


        //注册层次移动回调
        TextEngineHelper.getTextEngine()
            .registerTextEngineStatus(object : TextEngineJni.TextEngineStatus {
                override fun onPreviewInit(layerId: Int) {
                    Log.e("11111", "onPreviewInit :  layerId $layerId ")
                }

                override fun onTextLevelChange(isAdd: Boolean, layerId: Int, subTextId: Int) {

                }

                override fun onTextLayerAreaChange(
                    layerId: Int,
                    left: Float,
                    top: Float,
                    right: Float,
                    bottom: Float
                ) {
                    runOnUiThread {
                        Log.e(
                            "11111",
                            "onTextLayerAreaChange : left layerId $layerId " + left + "top: " + top + " right:" + right + "bottom: " + bottom
                        )
                        viewBinding.textRectManager.onChangeArea(layerId, left, top, right, bottom)
                    }
                }
            })


        viewBinding.btnMatrix.setOnClickListener {
            viewBinding.textRectManager.printAll()
            TextEngineHelper.getTextEngine()
                .printAll(TextEngineHelper.getTextEngine().TEXT_ENGINE_ID)
        }



        viewBinding.textRectManager.ITransformCallback = { id, tx, ty, s, r ->
            TextEngineHelper.getTextEngine().textLayerTransform(id, tx, ty, s, r)
        }

        //选中回调
        viewBinding.textRectManager.ISelectLayerCallBack = {
            tmpTextLayer = it
            it?.apply {
                viewBinding.sbFrame.progress = it.frameIndex;
            }
        }

        viewBinding.sbFrame.setOnSeekBarChangeListener(object :SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                tmpTextLayer?.apply {
                    this.frameIndex = progress;
                    TextEngineHelper.getTextEngine().updateTextLayerFrameIndex(layerId,frameIndex)
                }
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
            }

        })
    }

    var time = 0L
    var endTime = 0L

    private fun addSimpleText() {
        time = System.currentTimeMillis()
        tmpTextLayer = null
        tmpTextLayer = TextLayerManager.createTextLayer(this)
        tmpTextLayer?.apply {
            textList.forEach {
                TextEngineHelper.getTextEngine()
                    .addSimpleSubText(layerId, it.id, it.ttfPath, it.char, it.size, it.fontColor)
                viewBinding.textRectManager.addRect(this)
            }
        }

        FragmentHelp.showOrHideFragment(
            this@TextEditActivity,
            textStyleBottomSheetFragment
        )
    }


    override fun onDestroy() {
        super.onDestroy()
        TextLayerManager.clear()
    }

}