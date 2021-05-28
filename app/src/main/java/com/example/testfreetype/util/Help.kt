package com.example.testfreetype.util

import android.content.Context
import androidx.activity.OnBackPressedCallback
import androidx.appcompat.app.AppCompatActivity
import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentManager
import androidx.lifecycle.LifecycleOwner
import com.example.testfreetype.R
import com.example.testfreetype.TextEngineJni
import com.example.testfreetype.widget.TemplateFragment
import java.io.File

object PathHelp {
    fun getFontsPath(context: Context): String {
        val fontDir = File(StorageHelper.getExternalFilesDir(context), "fonts")
        fontDir.mkdir()
        return fontDir.absolutePath
    }

    fun getTemplatePath(context: Context): String {
        val fontDir = File(StorageHelper.getInternalFilesDir(context), "template")
        fontDir.mkdir()
        return fontDir.absolutePath
    }
}


object TextEngineHelper {
    private val textEngineJni by lazy {
        TextEngineJni()
    }

    fun getTextEngine(): TextEngineJni {
        return textEngineJni
    }

    fun init() {
        getTextEngine().init()
    }


}

object FragmentHelp {

    fun restoreFragment(activity: AppCompatActivity, tag: String?): Fragment? {
        val fm: FragmentManager = activity.supportFragmentManager
        return fm.findFragmentByTag(tag)
    }

    fun showOrHideFragment(activity: AppCompatActivity, fragment: Fragment) {
        val fm: FragmentManager = activity.supportFragmentManager
        if (fragment.isAdded) {
            if (fragment.isHidden) {
                fm.beginTransaction()
                    .setCustomAnimations(R.anim.fragment_b_show, R.anim.fragment_b_hide)
                    .show(fragment).commitAllowingStateLoss()
            } else {
                fm.beginTransaction()
                    .setCustomAnimations(R.anim.fragment_b_show, R.anim.fragment_b_hide)
                    .hide(fragment).commitAllowingStateLoss()
            }
            try {
                fm.executePendingTransactions()
            } catch (var4: IllegalStateException) {
            }
        }
    }

    fun showOrHideFragment(fm: FragmentManager, fragment: Fragment) {
        if (fragment.isAdded) {
            if (fragment.isHidden) {
                fm.beginTransaction()
                    .setCustomAnimations(R.anim.fragment_b_show, R.anim.fragment_b_hide)
                    .show(fragment).commitAllowingStateLoss()
            } else {
                fm.beginTransaction()
                    .setCustomAnimations(R.anim.fragment_b_show, R.anim.fragment_b_hide)
                    .hide(fragment).commitAllowingStateLoss()
            }
            try {
                fm.executePendingTransactions()
            } catch (var4: IllegalStateException) {
            }
        }
    }

    fun initFragment(
        activity: AppCompatActivity,
        fragment: Fragment,
        containerId: Int,
        tag: String?,
        isShow: Boolean
    ) {
        val fm: FragmentManager = activity.supportFragmentManager
        if (isShow) {
            fm.beginTransaction()
                .setCustomAnimations(R.anim.fragment_b_show, R.anim.fragment_b_hide)
                .add(containerId, fragment, tag).commitAllowingStateLoss()
        } else {
            fm.beginTransaction()
                .setCustomAnimations(R.anim.fragment_b_show, R.anim.fragment_b_hide)
                .add(containerId, fragment, tag)
                .hide(fragment).commit()
        }
        try {
            fm.executePendingTransactions()
        } catch (var9: java.lang.IllegalStateException) {
        }
    }

    fun isFragmentShowing(fragment: Fragment?): Boolean {
        return if (fragment == null) {
            false
        } else {
            fragment.isAdded && !fragment.isHidden
        }
    }

    fun addOnBackPressed(
        fragment: Fragment,
        owner: LifecycleOwner,
        onBackPressed: () -> Boolean
    ): OnBackPressedCallback {
        val callback = object : OnBackPressedCallback(true) {
            override fun handleOnBackPressed() {
                if (!isFragmentShowing(fragment)) {
                    isEnabled = false
                    fragment.requireActivity().onBackPressedDispatcher.onBackPressed()
                    isEnabled = true
                } else if (!onBackPressed()) {
                    isEnabled = false
                    fragment.requireActivity().onBackPressedDispatcher.onBackPressed()
                    isEnabled = true
                }
            }
        }
        fragment.requireActivity().onBackPressedDispatcher.addCallback(owner, callback)
        return callback
    }


    fun showTemplateFragment(
        activity: AppCompatActivity,
        contextId: Int,
        selectCallBack: (path: String) -> Unit,
        selectOkBack: () -> Unit
    ) {
        val tag = "TemplateFragment"
        var findFragmentByTag =
            activity.supportFragmentManager.findFragmentByTag(tag) as? TemplateFragment
        if (findFragmentByTag == null) {
            findFragmentByTag = TemplateFragment()
            findFragmentByTag.selectCallBack = selectCallBack
            findFragmentByTag.selectOk = selectOkBack;
            activity.supportFragmentManager.beginTransaction()
                .setCustomAnimations(R.anim.fragment_b_show, R.anim.fragment_b_hide)
                .add(contextId, findFragmentByTag, tag)
                .addToBackStack(null).commitAllowingStateLoss()
        } else {
            activity.supportFragmentManager.beginTransaction()
                .setCustomAnimations(R.anim.fragment_b_show, R.anim.fragment_b_hide)
                .show(findFragmentByTag)
                .addToBackStack(null).commitAllowingStateLoss();
        }
    }


    /*fun hide(activity: AppCompatActivity): Boolean {
        var findFragmentByTag =
            activity.supportFragmentManager.findFragmentByTag("TemplateFragment") as? TemplateFragment
        findFragmentByTag?.
        activity.supportFragmentManager.beginTransaction().
        Fragment feedCommentFragment = getFragmentManager ().findFragmentByTag("feedCommentFragment");
        getActivity().getSupportFragmentManager().beginTransaction()
            .setCustomAnimations(R.anim.comment_down_animation, R.anim.comment_down_animation)
            .hide(feedCommentFragment).commitAllowingStateLoss();


        return true
    }*/

}