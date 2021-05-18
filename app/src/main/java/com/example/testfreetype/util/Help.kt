package com.example.testfreetype.util

import android.content.Context
import androidx.appcompat.app.AppCompatActivity
import com.example.testfreetype.R
import com.example.testfreetype.widget.TemplateFragment
import java.io.File

object PathHelp {
    fun getFontsPath(context: Context): String {
        val fontDir = File(StorageHelper.getInternalFilesDir(context), "fonts")
        fontDir.mkdir()
        return fontDir.absolutePath
    }

    fun getTemplatePath(context: Context): String {
        val fontDir = File(StorageHelper.getInternalFilesDir(context), "template")
        fontDir.mkdir()
        return fontDir.absolutePath
    }
}

object FragmentHelp {

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