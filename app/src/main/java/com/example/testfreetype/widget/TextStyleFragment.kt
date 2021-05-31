package com.example.testfreetype.widget

import android.os.Bundle
import android.text.Editable
import android.text.TextWatcher
import android.util.Log
import android.view.View
import android.widget.SeekBar
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.GridLayoutManager
import by.kirich1409.viewbindingdelegate.viewBinding
import com.example.testfreetype.R
import com.example.testfreetype.adapter.TextFontAdapter
import com.example.testfreetype.bean.FontItem
import com.example.testfreetype.databinding.DialogTextBinding
import com.example.testfreetype.util.*
import java.io.File

class TextStyleFragment : Fragment(R.layout.dialog_text) {

    private val viewBinding by viewBinding(DialogTextBinding::bind)

    var styleCallBack: TextStyleFragment.OnStyleChange? = null

    var layerId: Int = 0

    var subTextId: Int = 0

    private val mFontList: List<FontItem> by lazy {
        val fontList =
            AssetsUtil.parseJsonToList(this.context, "fonts/fonts.json", FontItem::class.java)
        fontList[0].selected = true
        fontList
    }

    private val textFontAdapter: TextFontAdapter by lazy {
        TextFontAdapter(this.context, mFontList,
            TextFontAdapter.OnFontSelectListener { position ->
                var fontPath = mFontList[position].fontPath
                fontPath =
                    PathHelp.getFontsPath(requireContext()) + File.separator + fontPath

                styleCallBack?.changeFont(fontPath)
            })
    }


    private val textWatcher = object : TextWatcher {
        override fun afterTextChanged(s: Editable?) {
        }

        override fun beforeTextChanged(text: CharSequence?, start: Int, count: Int, after: Int) {
        }

        override fun onTextChanged(text: CharSequence?, start: Int, before: Int, count: Int) {
            styleCallBack?.changeText(text.toString())
        }
    }


    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        viewBinding.edText.addTextChangedListener(textWatcher)

        viewBinding.rgLine.setOnCheckedChangeListener { group, checkedId ->
            styleCallBack?.changeHorizontal(checkedId == R.id.rb_horizontal)

        }
        viewBinding.xSeekBar
            .setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
                override fun onProgressChanged(
                    seekBar: SeekBar?,
                    progress: Int,
                    fromUser: Boolean
                ) {
                    styleCallBack?.changeWordSpacing(progress)
                }

                override fun onStartTrackingTouch(seekBar: SeekBar?) {
                }

                override fun onStopTrackingTouch(seekBar: SeekBar?) {
                }
            })

        viewBinding.ySeekBar
            .setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
                override fun onProgressChanged(
                    seekBar: SeekBar?,
                    progress: Int,
                    fromUser: Boolean
                ) {
                    styleCallBack?.changeLineWordSpacing(progress)
                }

                override fun onStartTrackingTouch(seekBar: SeekBar?) {
                }

                override fun onStopTrackingTouch(seekBar: SeekBar?) {
                }
            })

        viewBinding.sizeSeekBar
            .setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
                override fun onProgressChanged(
                    seekBar: SeekBar?,
                    progress: Int,
                    fromUser: Boolean
                ) {
                }

                override fun onStartTrackingTouch(seekBar: SeekBar?) {
                }

                override fun onStopTrackingTouch(seekBar: SeekBar?) {
                    seekBar?.apply {
                        styleCallBack?.changeFontSize(progress)
                    }
                }
            })

        viewBinding.distanceSeekBar.setOnSeekBarChangeListener(object :
            SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(
                seekBar: SeekBar?,
                progress: Int,
                fromUser: Boolean
            ) {
                seekBar?.apply {
                    val outLine =
                        0.465f - (progress.toFloat() / max) * 0.465f
                    styleCallBack?.changeDistance(outLine)
                }
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        viewBinding.outDistanceSeekBar
            .setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
                override fun onProgressChanged(
                    seekBar: SeekBar?,
                    progress: Int,
                    fromUser: Boolean
                ) {
                    seekBar?.apply {
                        val outLine =
                            0.5f - (progress.toFloat() / max) * 0.5f
                        styleCallBack?.changeOutlineDistanceMark(outLine)
                    }
                }

                override fun onStartTrackingTouch(seekBar: SeekBar?) {
                }

                override fun onStopTrackingTouch(seekBar: SeekBar?) {

                }
            })

        viewBinding.fontColorSeekBar
            .setOnColorPickerChangeListener(object :
                ColorPickerView.OnColorPickerChangeListener {
                override fun onStartTrackingTouch(picker: ColorPickerView?) {
                }

                override fun onColorChanged(picker: ColorPickerView?, color: Int) {
                    styleCallBack?.changeFontColor(color)
                }

                override fun onStopTrackingTouch(picker: ColorPickerView?) {

                }

            })
        viewBinding.stokeColorSeekBar
            .setOnColorPickerChangeListener(object :
                ColorPickerView.OnColorPickerChangeListener {
                override fun onStartTrackingTouch(picker: ColorPickerView?) {
                }

                override fun onColorChanged(picker: ColorPickerView?, color: Int) {
                    styleCallBack?.changeOutlineColor(color)
                }

                override fun onStopTrackingTouch(picker: ColorPickerView?) {

                }

            })
        viewBinding.shadowColorSeekBar
            .setOnColorPickerChangeListener(object :
                ColorPickerView.OnColorPickerChangeListener {
                override fun onStartTrackingTouch(picker: ColorPickerView?) {
                }

                override fun onColorChanged(picker: ColorPickerView?, color: Int) {
                    styleCallBack?.changeShadowColor(color)
                }

                override fun onStopTrackingTouch(picker: ColorPickerView?) {

                }

            })

        viewBinding.zSeekBar
            .setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
                override fun onProgressChanged(
                    seekBar: SeekBar?,
                    progress: Int,
                    fromUser: Boolean
                ) {
                    seekBar?.apply {
                        styleCallBack?.changeShadowDistance(progress.toFloat())
                    }
                }

                override fun onStartTrackingTouch(seekBar: SeekBar?) {
                }

                override fun onStopTrackingTouch(seekBar: SeekBar?) {

                }
            })
        viewBinding.angleSeekBar
            .setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
                override fun onProgressChanged(
                    seekBar: SeekBar?,
                    progress: Int,
                    fromUser: Boolean
                ) {
                    seekBar?.apply {
                        styleCallBack?.changeShadowAngle(progress)
                    }
                }

                override fun onStartTrackingTouch(seekBar: SeekBar?) {
                }

                override fun onStopTrackingTouch(seekBar: SeekBar?) {

                }
            })
        viewBinding.alphaSeekBar
            .setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
                override fun onProgressChanged(
                    seekBar: SeekBar?,
                    progress: Int,
                    fromUser: Boolean
                ) {
                    seekBar?.apply {
                        val shadowAlpha =
                            0.5f - (progress.toFloat() / max) * 0.5f

                        styleCallBack?.changeShadowAlpha(shadowAlpha)
                    }
                }

                override fun onStartTrackingTouch(seekBar: SeekBar?) {
                }

                override fun onStopTrackingTouch(seekBar: SeekBar?) {

                }
            })
        viewBinding.ivYes.setOnClickListener {
            viewBinding.edText.removeTextChangedListener(textWatcher)
            viewBinding.edText.setText("")
            viewBinding.edText.addTextChangedListener(textWatcher)
            styleCallBack?.onCreateLayer()
            if (FragmentHelp.isFragmentShowing(this))
                FragmentHelp.showOrHideFragment(requireActivity().supportFragmentManager, this)
        }

        val rvFontList = viewBinding.rvFontList
        rvFontList.layoutManager = GridLayoutManager(this.context, 4)
        rvFontList.adapter = textFontAdapter

        FragmentHelp.addOnBackPressed(this, this) {
            if (FragmentHelp.isFragmentShowing(this)) {
                FragmentHelp.showOrHideFragment(requireActivity().supportFragmentManager, this)
                TextEngineHelper.getTextEngine().cleanPreview()
                styleCallBack?.exit()
            }
            return@addOnBackPressed true
        }

        Log.e("11111", "oncreateview")
    }

    override fun onHiddenChanged(hidden: Boolean) {
        super.onHiddenChanged(hidden)
        Log.e("11111", "onHiddenChanged $hidden")
    }


    fun setLayer(layerId: Int, subTextId: Int) {
        this.layerId = layerId
        this.subTextId = subTextId
    }


    interface OnStyleChange {
        fun changeText(text: String)
        fun changeHorizontal(horizontal: Boolean)
        fun changeWordSpacing(spacing: Int)
        fun changeLineWordSpacing(spacing: Int)
        fun changeFont(fontPath: String)
        fun changeFontSize(pixie: Int)
        fun changeFontColor(color: Int);
        fun changeDistance(distanceMark: Float)
        fun changeOutlineDistanceMark(outLineDistanceMark: Float)
        fun changeOutlineColor(color: Int)
        fun changeShadowDistance(shadowDistance: Float)
        fun changeShadowAlpha(shadowAlpha: Float)
        fun changeShadowColor(shadowColor: Int)
        fun changeShadowAngle(shadowAngle: Int)
        fun onCreateLayer()
        fun exit()
    }
}