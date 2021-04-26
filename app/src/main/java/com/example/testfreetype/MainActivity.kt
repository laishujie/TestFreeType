package com.example.testfreetype

import android.Manifest
import android.content.Intent
import android.graphics.Color
import android.os.Build
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.example.testfreetype.databinding.ActivityMainBinding
import com.example.testfreetype.util.StorageHelper
import java.io.*

class MainActivity : AppCompatActivity() {


    val testFilePath by lazy {
        File(filesDir, "sdf_test.png").path
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val binding = ActivityMainBinding.inflate(layoutInflater)

        setContentView(binding.root)
        binding.btnCreateSdf.setOnClickListener {
            val sdfPath = StorageHelper.getDCIMPath(this) + File.separator + "${System.currentTimeMillis()}.png"
            val srcPath = File(filesDir, "test_lai_w.png").path
            copyAsset("test_lai_w.png")
            TypeJni.testOpengGlSDF(srcPath, sdfPath, 10f, 1f)
            Log.e("1111", sdfPath)
        }

        binding.btnTextFont.setOnClickListener {
            val intent= Intent(this,TestFontTextureActivity::class.java)
            copyAsset("sdf_test.png")

            intent.putExtra("path",testFilePath)
            startActivity(intent)
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(
                arrayOf(
                    Manifest.permission.READ_EXTERNAL_STORAGE,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE
                ), 0
            )
        }

    }

    private fun copyAsset(assetFilePath: String) {
        if (File(assetFilePath).exists()) return

        var `is`: InputStream? = null
        var os: OutputStream? = null

        try {
            `is` = assets.open(assetFilePath)
            os = FileOutputStream(File(filesDir, assetFilePath))
            val buffer = ByteArray(1024)
            var length: Int
            while (`is`.read(buffer).also { length = it } > 0) {
                os.write(buffer, 0, length)
            }
            `is`.close()
            os.close()
        } catch (e: Exception) {
            e.printStackTrace()
        } finally {
            if (`is` != null) {
                try {
                    `is`.close()
                } catch (e: IOException) {
                    e.printStackTrace()
                }
            }
            if (os != null) {
                try {
                    os.close()
                } catch (e: IOException) {
                    e.printStackTrace()
                }
            }
        }
    }
}