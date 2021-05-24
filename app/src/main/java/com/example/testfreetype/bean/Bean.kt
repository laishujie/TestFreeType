package com.example.testfreetype.bean

import android.graphics.Color
import android.graphics.RectF
import android.util.SparseArray
import com.google.gson.annotations.SerializedName

data class TextInfo(
    var id: Int,
    var char: String?,
    var ttfPath: String?,
    var fontColor: Int = Color.WHITE,
    var outLineColor: Int = Color.YELLOW,
    var shadowColor: Int = Color.GRAY,
    var shadowDistance: Float = 0f,
    var shadowAngle: Int = 0,
    var distanceMark: Float = 0.5f,
    var outLineDistanceMark: Float = 0.5f,
    var spacing: Int = 0,
    var lineSpacing: Int = 0,
    var shadowAlpha: Float = 0.5f,
    var size: Int = 72,
    var horizontal: Boolean = true
)

data class TextLayer(
    var textInfo: TextInfo,
    //层id
    var layerId: Int = -1,
    var isTemplate: Boolean = false,
    var tempConfigPath: String? = null,
    var fontFolder: String? = null,
    var rect: RectF = RectF()
)

data class MatrixInfo(var tx: Float, var ty: Float, var scale: Float, var rangle: Float)

data class FontItem(
    @field:SerializedName("path") var fontPath: String,
    @field:SerializedName(
        "name"
    ) var fontName: String,
    var selected: Boolean = false
)

data class ImgItem(
    @field:SerializedName("path") var path: String,
    @field:SerializedName(
        "imagePath"
    ) var imagePath: String,
    var selected: Boolean = false
)


object LayerManager {

    var currLayerId = -1
    var layers = SparseArray<TextLayer>()


    fun getPreViewTextInfo() {

    }

    fun getLayer(id: Int): TextLayer? {
        return layers[id]
    }

    fun addLayer(id: Int, textInfo: TextLayer) {
        layers.append(id, textInfo)
        currLayerId = id
    }

    fun getStringSame(src: String?, des: String?): String? {
        if (src == des) return null
        return des
    }


    fun clear() {
        currLayerId = -1
        layers.clear()
    }
}