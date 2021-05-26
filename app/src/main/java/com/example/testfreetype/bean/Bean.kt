package com.example.testfreetype.bean

import kotlin.reflect.KClass
import kotlin.reflect.full.declaredMemberProperties
import kotlin.reflect.full.primaryConstructor

import android.content.Context
import android.graphics.Color
import android.graphics.RectF
import android.util.SparseArray
import com.example.testfreetype.util.PathHelp
import com.google.gson.annotations.SerializedName
import java.io.File

data class TextInfo(
    var id: Int,
    var char: String,
    var ttfPath: String,
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


fun <T : Any> T.deepCopy(): T {
    if (!this::class.isData) {
        return this
    }

    return this::class.primaryConstructor!!.let { primaryConstructor ->
        primaryConstructor.parameters
            .map { parameter ->
                val value =
                    (this::class as KClass<T>).declaredMemberProperties.first { it.name == parameter.name }
                        .get(this)
                if ((parameter.type.classifier as? KClass<*>)?.isData == true) {
                    parameter to value?.deepCopy()
                } else {
                    parameter to value
                }
            }
            .toMap()
            .let(primaryConstructor::callBy)
    }
}


data class TextLayer(
    var textInfo: TextInfo?,
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


object TextLayerManager {

    var currLayerId = -1

    var layers = SparseArray<TextLayer?>()

    var layerIds = 0

    fun addSimpleText(textLayer: TextLayer, layerId: Int, subId: Int) {
        textLayer.layerId = layerId
        textLayer.textInfo?.id = subId
        layers.put(layerId, textLayer)
    }


    fun getTextLayer(layerId: Int): TextLayer? {
        return layers[layerId]
    }

    fun createTextLayer(context: Context): TextLayer {
        layerIds++
        val textLayer = TextLayer(createSubText(context))
        textLayer.layerId = layerIds
        return textLayer
    }

    fun createTextLayer(textInfo: TextInfo): TextLayer {
        return TextLayer(textInfo)
    }

    fun createSubText(context: Context): TextInfo {
        return TextInfo(
            0,
            "输入文字",
            PathHelp.getFontsPath(context) + File.separator + "DroidSansFallback.ttf"
        )
    }


    fun clear() {
        currLayerId = -1
        layers.clear()
    }
}