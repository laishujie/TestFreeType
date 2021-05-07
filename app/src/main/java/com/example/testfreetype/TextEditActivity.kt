package com.example.testfreetype

import android.os.Bundle
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import com.example.testfreetype.databinding.ActivityTextEditBinding
import com.example.testfreetype.util.KeyboardUtils
import com.example.testfreetype.util.SizeUtils
import com.example.testfreetype.util.SoftInputUtil
import com.example.testfreetype.util.TextEditSurfaceManager
import com.example.testfreetype.widget.TextStyleBottomSheetFragment


class TextEditActivity : AppCompatActivity() {

    private val textEdit = TextEditSurfaceManager()
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

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val inflate = ActivityTextEditBinding.inflate(layoutInflater)
        setContentView(inflate.root)

        inflate.surfaceView.surfaceTextureListener = textEdit


        inflate.btnText.setOnClickListener {
            textStyleBottomSheetFragment.show(
                supportFragmentManager,
                "TextStyleBottomSheetFragment"
            )
        }

        textStyleBottomSheetFragment.styleCallBack =
            object : TextStyleBottomSheetFragment.OnStyleChange {
                override fun change(style: TextStyleBottomSheetFragment.Style) {
                    textEdit.drawText(ttfPath, style.char, "")
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