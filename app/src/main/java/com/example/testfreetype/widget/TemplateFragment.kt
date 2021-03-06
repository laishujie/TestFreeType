package com.example.testfreetype.widget

import android.os.Bundle
import android.util.Log
import android.view.View
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.GridLayoutManager
import by.kirich1409.viewbindingdelegate.viewBinding
import com.example.testfreetype.R
import com.example.testfreetype.adapter.TmpAdapter
import com.example.testfreetype.bean.ImgItem
import com.example.testfreetype.databinding.FragmentTemplateBinding
import com.example.testfreetype.util.AssetsUtil
import com.example.testfreetype.util.FragmentHelp
import com.example.testfreetype.util.PathHelp
import com.example.testfreetype.util.TextEngineHelper
import java.io.File

class TemplateFragment : Fragment(R.layout.fragment_template) {

    private val viewBinding by viewBinding(FragmentTemplateBinding::bind)
    var selectCallBack: ((String) -> Unit?)? = null
    var exitOk: (() -> Unit?)? = null


    private val imgList: List<ImgItem> by lazy {
        val fontList =
            AssetsUtil.parseJsonToList(
                requireContext(),
                "template/templates.json",
                ImgItem::class.java
            )
        fontList
    }


    private val imgAdapter: TmpAdapter by lazy {
        TmpAdapter(this.context, imgList,
            TmpAdapter.OnImgSelectListener { position ->
                val imgItem = imgList[position]
                val file = File(PathHelp.getTemplatePath(requireContext()), imgItem.path)
                selectCallBack?.invoke(file.absolutePath)
            })
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        viewBinding.rvList.layoutManager = GridLayoutManager(requireContext(), 4)
        viewBinding.rvList.adapter = imgAdapter
        viewBinding.ivYes.setOnClickListener {
            requireActivity().supportFragmentManager.beginTransaction()
                .setCustomAnimations(R.anim.fragment_b_show, R.anim.fragment_b_hide).hide(this)
                .commitAllowingStateLoss()
        }

        FragmentHelp.addOnBackPressed(this, this) {
            if(FragmentHelp.isFragmentShowing(this)){
                FragmentHelp.showOrHideFragment(requireActivity().supportFragmentManager, this)
                exitOk?.invoke()
            }

            return@addOnBackPressed true
        }
    }


}