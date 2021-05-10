package com.example.testfreetype.bean

import android.graphics.Color
import android.util.SparseArray
import com.google.gson.annotations.SerializedName

data class TextLayer(
    var id: Int,
    var char: String?,
    var ttfPath: String?,
    var fontColor: Int = Color.WHITE,
    var distanceMark: Float = 0.5f,
    var outLineDistanceMark: Float = 0.5f,
    var spacing: Int = 0,
    var lineSpacing: Int = 0,
    var size: Int = 72,
    var horizontal: Boolean = true
) {
}

data class FontItem(
    @field:SerializedName("path") var fontPath: String,
    @field:SerializedName(
        "name"
    ) var fontName: String,
    var selected: Boolean = false
)

object TextConfigManager {

    var currId = 0

    var layers = SparseArray<TextLayer>()


    fun getLayer(id: Int): TextLayer? {
        return layers[id]
    }

    fun addLayer(ttfPath: String, text: String): Int {

        layers.append(currId, TextLayer(currId, text, ttfPath))

        currId++

        return layers[layers.size() - 1].id
    }

    fun configLayer(id: Int, text: String? = null, ttfPath: String?, horizontal: Boolean = true) {
        val textLayer = layers[id]
        if (textLayer != null) {
            textLayer.char = text
            textLayer.horizontal = horizontal
            textLayer.ttfPath = ttfPath
        }
    }

    fun getStringSame(src: String?, des: String?): String? {
        if (src == des) return null
        return des
    }


    fun clear() {
        currId = 0
        layers.clear()
    }
}