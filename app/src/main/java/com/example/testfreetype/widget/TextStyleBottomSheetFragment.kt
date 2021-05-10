package com.example.testfreetype.widget

import android.app.Dialog
import android.graphics.Color
import android.graphics.drawable.ColorDrawable
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.view.Window
import android.widget.EditText
import android.widget.RadioGroup
import android.widget.SeekBar
import androidx.core.widget.addTextChangedListener
import androidx.recyclerview.widget.GridLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.example.testfreetype.R
import com.example.testfreetype.adapter.TextFontAdapter
import com.example.testfreetype.bean.FontItem
import com.example.testfreetype.util.AssetsUtil
import com.example.testfreetype.util.SizeUtils
import com.example.testfreetype.util.StorageHelper
import com.google.android.material.bottomsheet.BottomSheetDialog
import com.google.android.material.bottomsheet.BottomSheetDialogFragment
import java.io.File


class TextStyleBottomSheetFragment : BottomSheetDialogFragment() {

    var styleCallBack: OnStyleChange? = null;

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
                    StorageHelper.getInternalFilesDir(this.context).absolutePath + File.separator + fontPath

                styleCallBack?.changeFont(fontPath)
            })
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
    }


    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        dialog?.apply {
            requestWindowFeature(Window.FEATURE_NO_TITLE)
            window?.setBackgroundDrawable(ColorDrawable(Color.TRANSPARENT))
        }
        return inflater.inflate(R.layout.dialog_text, container)
    }


    override fun onCreateDialog(savedInstanceState: Bundle?): Dialog {
        val bottomSheetDialog = BottomSheetDialog(context!!, R.style.DialogFullScreen)
        bottomSheetDialog.behavior.peekHeight = SizeUtils.dp2px(300f)
        return bottomSheetDialog
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)



        view.findViewById<EditText>(R.id.ed_text).addTextChangedListener(
            onTextChanged = { text, start, count, after ->
                styleCallBack?.changeText(text.toString())
            }
        )

        view.findViewById<RadioGroup>(R.id.rg_line).setOnCheckedChangeListener { group, checkedId ->
            styleCallBack?.changeHorizontal(checkedId == R.id.rb_horizontal)

        }
        view.findViewById<SeekBar>(R.id.x_seekBar)
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

        view.findViewById<SeekBar>(R.id.y_seekBar)
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

        view.findViewById<SeekBar>(R.id.size_seekBar)
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

        view.findViewById<SeekBar>(R.id.distance_seekBar)
            .setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
                override fun onProgressChanged(
                    seekBar: SeekBar?,
                    progress: Int,
                    fromUser: Boolean
                ) {
                    seekBar?.apply {
                        styleCallBack?.changeDistance(progress.toFloat() / max)
                    }
                }

                override fun onStartTrackingTouch(seekBar: SeekBar?) {
                }

                override fun onStopTrackingTouch(seekBar: SeekBar?) {

                }
            })

        view.findViewById<SeekBar>(R.id.out_distance_seekBar)
            .setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
                override fun onProgressChanged(
                    seekBar: SeekBar?,
                    progress: Int,
                    fromUser: Boolean
                ) {
                    seekBar?.apply {
                        styleCallBack?.changeOutlineDistanceMark(progress.toFloat() / max)
                    }
                }

                override fun onStartTrackingTouch(seekBar: SeekBar?) {
                }

                override fun onStopTrackingTouch(seekBar: SeekBar?) {

                }
            })


        view.findViewById<ColorPickerView>(R.id.font_color_seekBar)
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


        val rvFontList = view.findViewById<RecyclerView>(R.id.rv_font_list)
        rvFontList.layoutManager = GridLayoutManager(this.context, 4)
        rvFontList.adapter = textFontAdapter
    }

}