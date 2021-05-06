package com.example.testfreetype

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.core.widget.addTextChangedListener
import com.example.testfreetype.databinding.ActivityTextEditBinding
import com.example.testfreetype.util.TextEditSurfaceManager

class TextEditActivity : AppCompatActivity() {

    private val textEdit = TextEditSurfaceManager()

    val ttfPath by lazy {
        intent.getStringExtra("path").toString()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val inflate = ActivityTextEditBinding.inflate(layoutInflater)
        setContentView(inflate.root)


        inflate.surfaceView.surfaceTextureListener = textEdit

      /*inflate.edText.addTextChangedListener(onTextChanged = { text, start, count, after ->
            textEdit.drawText(ttfPath, text.toString())
        })*/

        inflate.btnOk.setOnClickListener {
            textEdit.drawText(ttfPath, inflate.edText.text.toString())
        }
    }

    override fun onDestroy() {
        super.onDestroy()
    }
}