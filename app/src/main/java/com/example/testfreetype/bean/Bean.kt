package com.example.testfreetype.bean

import android.content.Context
import android.graphics.Color
import android.graphics.RectF
import android.util.SparseArray
import com.example.testfreetype.util.AssetsUtil
import com.example.testfreetype.util.PathHelp
import com.google.gson.annotations.SerializedName
import java.io.File
import kotlin.reflect.KClass
import kotlin.reflect.full.declaredMemberProperties
import kotlin.reflect.full.primaryConstructor

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
    var horizontal: Boolean = true,
    var file: String = "",//图片路径
    var offsetX: Float = 0f,
    var offsetY: Float = 0f,
    var isTextImage: Boolean = false
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
    var textList: ArrayList<TextInfo> = ArrayList(),
    //层id
    var layerId: Int = -1,
    var isTemplate: Boolean = false,
    var templateFolder: String? = null,
    var fontFolder: String? = null,
    var rect: RectF = RectF()
) {
    fun getFirst(): TextInfo? {
        if (textList.isEmpty())
            return null
        return textList.first()
    }
}

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
    var layerSubIds = 0


    fun getTextLayer(layerId: Int): TextLayer? {
        return layers[layerId]
    }

    fun createTextLayer(context: Context): TextLayer {
        layerIds++
        val textLayer = TextLayer(createSubText(context))
        textLayer.layerId = layerIds
        return textLayer
    }

    private fun createSubText(context: Context): ArrayList<TextInfo> {
        layerSubIds++
        return arrayListOf(
            TextInfo(
                layerSubIds,
                "输入文字",
                PathHelp.getFontsPath(context) + File.separator + "DroidSansFallback.ttf"
            )
        )
    }

    fun createByJson(context: Context, json: File): TextLayer? {
        layerIds++
        val parseJsonToList = AssetsUtil.parseJsonToObject(context, json, TextLayerJson::class.java)
        val textLayer = TextLayer()
        textLayer.layerId = layerIds
        val fontPath = PathHelp.getFontsPath(context) + File.separator
        textLayer.templateFolder = json.parent
        parseJsonToList.ts.forEach {

            val text = if (it.wenan == null || it.wenan!!.isEmpty()) "" else it.wenan!![0]

            val textInfo = if (it.classX == "文字") {
                TextInfo(
                    it.cid.toInt(),
                    text,
                    fontPath + it.font,
                    Color.parseColor("#" + it.color),
                    size = it.size.toInt(),
                    offsetX = it.offsetX.toFloat(),
                    offsetY = it.offsetY.toFloat(),
                    isTextImage = false
                )
            } else {
                TextInfo(
                    id = it.cid.toInt(),
                    char = text,
                    ttfPath = "",
                    offsetX = it.offsetX.toFloat(),
                    offsetY = it.offsetY.toFloat(),
                    file = it.file,
                    isTextImage = true
                )
            }
            textLayer.textList.add(textInfo)
        }
        textLayer.isTemplate = true

        if (textLayer.textList.isNotEmpty()) {
            return textLayer
        }
        return null
    }


    fun clear() {
        currLayerId = -1
        layers.clear()
    }
}


data class TextLayerJson(
    @SerializedName("animation_id")
    var animationId: String,
    @SerializedName("animation_json")
    var animationJson: String,
    @SerializedName("bgImg")
    var bgImg: BgImg,
    @SerializedName("displayRefW")
    var displayRefW: Int,
    @SerializedName("displayW")
    var displayW: Int,
    @SerializedName("effect_id")
    var effectId: String,
    @SerializedName("offset_x")
    var offsetX: String,
    @SerializedName("offset_y")
    var offsetY: String,
    @SerializedName("refH")
    var refH: Int,
    @SerializedName("refW")
    var refW: Int,
    @SerializedName("t_pos")
    var tPos: String,
    @SerializedName("ts")
    var ts: List<T>
)

data class BgImg(
    @SerializedName("animation_json")
    var animationJson: String,
    @SerializedName("bgImg")
    var bgImg: String,
    @SerializedName("bgImgFillH")
    var bgImgFillH: Boolean,
    @SerializedName("bgImgFillV")
    var bgImgFillV: Boolean,
    @SerializedName("bgImgPaddingBottom")
    var bgImgPaddingBottom: Int,
    @SerializedName("bgImgPaddingLeft")
    var bgImgPaddingLeft: Int,
    @SerializedName("bgImgPaddingRight")
    var bgImgPaddingRight: Int,
    @SerializedName("bgImgPaddingTop")
    var bgImgPaddingTop: Int
)

data class T(
    @SerializedName("file")
    var file: String,
    @SerializedName("align")
    var align: String,
    @SerializedName("animation_id")
    var animationId: String,
    @SerializedName("animation_json")
    var animationJson: String,
    @SerializedName("autoSize")
    var autoSize: String,
    @SerializedName("bgImg")
    var bgImg: BgImgX,
    @SerializedName("blur")
    var blur: String,
    @SerializedName("blurColor")
    var blurColor: String,
    @SerializedName("blurRadius")
    var blurRadius: String,
    @SerializedName("cid")
    var cid: String,
    @SerializedName("class")
    var classX: String,
    @SerializedName("color")
    var color: String,
    @SerializedName("con")
    var con: String,
    @SerializedName("cycle")
    var cycle: String,
    @SerializedName("effect_id")
    var effectId: String,
    @SerializedName("font")
    var font: String,
    @SerializedName("fontStyle")
    var fontStyle: String,
    @SerializedName("font_x_off")
    var fontXOff: String,
    @SerializedName("font_y_off")
    var fontYOff: String,
    @SerializedName("maxLine")
    var maxLine: String,
    @SerializedName("maxNum")
    var maxNum: String,
    @SerializedName("nc_color")
    var ncColor: String,
    @SerializedName("offset_x")
    var offsetX: String,
    @SerializedName("offset_y")
    var offsetY: String,
    @SerializedName("pos")
    var pos: String,
    @SerializedName("refH")
    var refH: Int,
    @SerializedName("refW")
    var refW: Int,
    @SerializedName("rotation")
    var rotation: String,
    @SerializedName("shadow_c")
    var shadowC: String,
    @SerializedName("shadow_r")
    var shadowR: String,
    @SerializedName("shadow_x")
    var shadowX: String,
    @SerializedName("shadow_y")
    var shadowY: String,
    @SerializedName("size")
    var size: String,
    @SerializedName("textureImg")
    var textureImg: String,
    @SerializedName("typeset")
    var typeset: String,
    @SerializedName("verticalspacing")
    var verticalspacing: String,
    @SerializedName("wenan")
    var wenan: List<String>?,
    @SerializedName("wordspace")
    var wordspace: String
)

data class BgImgX(
    @SerializedName("animation_json")
    var animationJson: String,
    @SerializedName("bgImg")
    var bgImg: String,
    @SerializedName("bgImgPaddingBottom")
    var bgImgPaddingBottom: Int,
    @SerializedName("bgImgPaddingLeft")
    var bgImgPaddingLeft: Int,
    @SerializedName("bgImgPaddingRight")
    var bgImgPaddingRight: Int,
    @SerializedName("bgImgPaddingTop")
    var bgImgPaddingTop: Int
)