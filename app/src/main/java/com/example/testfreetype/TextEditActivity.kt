package com.example.testfreetype

import android.graphics.Typeface
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import by.kirich1409.viewbindingdelegate.viewBinding
import com.example.testfreetype.bean.TextInfo
import com.example.testfreetype.databinding.ActivityTextEditBinding
import com.example.testfreetype.util.FragmentHelp
import com.example.testfreetype.util.PathHelp
import com.example.testfreetype.util.StorageHelper
import com.example.testfreetype.util.TextEditSurfaceManager
import com.example.testfreetype.widget.TextStyleBottomSheetFragment
import java.io.File


class TextEditActivity : AppCompatActivity(R.layout.activity_text_edit) {


    private val editSurfaceManager = TextEditSurfaceManager()

    //当前预览的textInfo
    private val textPreView by lazy {
        TextInfo(-1, "", ttfPath)
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

    private val textStyleBottomSheetFragment by lazy {
        var fragment =
            supportFragmentManager.findFragmentByTag("TextStyleBottomSheetFragment") as? TextStyleBottomSheetFragment
        if (fragment == null) {
            fragment = TextStyleBottomSheetFragment()
        }
        fragment!!
    }

    private val viewBinding by viewBinding(ActivityTextEditBinding::bind)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

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

            if (textStyleBottomSheetFragment.isAdded) {
                supportFragmentManager.beginTransaction().remove(textStyleBottomSheetFragment)
                    .commit()
            }
            textStyleBottomSheetFragment.show(
                supportFragmentManager,
                "TextStyleBottomSheetFragment"
            )
        }

        /* viewBinding.btnTestJson.setOnClickListener {
             if(File(jsonPath).exists())
             editSurfaceManager.testJson(jsonPath,fontPath)
         }*/

        viewBinding.tvTemplate.setOnClickListener {
            FragmentHelp.showTemplateFragment(this, R.id.frame_menu, selectCallBack = { path ->
                editSurfaceManager.drawPreViewLayerByJson(path, fontPath)
            }, selectOkBack = {
                editSurfaceManager.addThePreviewLayerByJson2Map()
            })
        }

        textStyleBottomSheetFragment.styleCallBack =
            object : TextStyleBottomSheetFragment.OnStyleChange {
                override fun changeText(text: String) {
                    editSurfaceManager
                    textPreView.apply {
                        char = text
                        editSurfaceManager.drawPreViewLayer(this)
                    }
                }

                override fun changeHorizontal(horizontal: Boolean) {
                    textPreView.apply {
                        this.horizontal = horizontal
                        editSurfaceManager.drawPreViewLayer(this)
                    }
                }

                override fun changeWordSpacing(spacing: Int) {
                    textPreView.apply {
                        this.spacing = spacing
                        editSurfaceManager.drawPreViewLayer(this)
                    }
                }

                override fun changeLineWordSpacing(spacing: Int) {
                    textPreView.apply {
                        this.lineSpacing = spacing
                        editSurfaceManager.drawPreViewLayer(this)
                    }
                }

                override fun changeFont(fontPath: String) {
                    textPreView.apply {
                        this.ttfPath = fontPath
                        editSurfaceManager.drawPreViewLayer(this)
                    }
                }

                override fun changeFontSize(pixie: Int) {
                    textPreView.apply {
                        this.size = if (pixie == 0) 1 else pixie
                        editSurfaceManager.drawPreViewLayer(this)
                    }
                }

                override fun changeFontColor(color: Int) {
                    textPreView.apply {
                        this.fontColor = color
                        editSurfaceManager.drawPreViewLayer(this)
                    }
                }

                override fun changeDistance(distanceMark: Float) {
                    textPreView.apply {
                        this.distanceMark = distanceMark
                        editSurfaceManager.drawPreViewLayer(this)
                    }
                }

                override fun changeOutlineDistanceMark(outLineDistanceMark: Float) {
                    textPreView.apply {
                        this.outLineDistanceMark = outLineDistanceMark
                        editSurfaceManager.drawPreViewLayer(this)
                    }
                }

                override fun changeOutlineColor(color: Int) {
                    textPreView.apply {
                        this.outLineColor = color
                        editSurfaceManager.drawPreViewLayer(this)
                    }
                }

                override fun changeShadowDistance(shadowDistance: Float) {
                    textPreView.apply {
                        this.shadowDistance = shadowDistance
                        editSurfaceManager.drawPreViewLayer(this)
                    }
                }

                override fun changeShadowAlpha(shadowAlpha: Float) {
                    textPreView.apply {
                        this.shadowAlpha = shadowAlpha
                        editSurfaceManager.drawPreViewLayer(this)
                    }
                }

                override fun changeShadowColor(shadowColor: Int) {
                    textPreView.apply {
                        this.shadowColor = shadowColor
                        editSurfaceManager.drawPreViewLayer(this)
                    }
                }

                override fun changeShadowAngle(shadowAngle: Int) {
                    textPreView.apply {
                        this.shadowAngle = shadowAngle
                        editSurfaceManager.drawPreViewLayer(this)
                    }
                }

                override fun onCreateLayer() {
                    //TODO 这里应该需要重新复制临时配置
                    textPreView.char = null
                    editSurfaceManager.addThePreviewLayer2Map()
                    textStyleBottomSheetFragment.dismissAllowingStateLoss()
                }
            }

        /*softInputUtil.attachSoftInput(
            viewBinding.btnText
        ) { isSoftInputShow, _, viewOffset ->
            if (isSoftInputShow) {
                viewBinding.btnText.translationY = viewBinding.btnText.translationY - viewOffset - SizeUtils.dp2px(10f)
                viewBinding.viewBg.translationY = viewBinding.btnText.translationY
                //viewBinding.surfaceView.translationY= viewBinding.btnText.translationY
            } else {
                viewBinding.btnText.translationY = 0f
                viewBinding.viewBg.translationY=0f
                //viewBinding.surfaceView.translationY= 0f
            }
        }*/
    }

    override fun onBackPressed() {

        super.onBackPressed()
    }


}