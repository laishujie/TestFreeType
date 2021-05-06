package com.example.testfreetype

import android.Manifest
import android.content.Intent
import android.os.Build
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.example.testfreetype.databinding.ActivityMainBinding
import com.example.testfreetype.util.StorageHelper
import java.io.File

class MainActivity : AppCompatActivity() {

    private val sdfPath by lazy {
        StorageHelper.getDownloadPath(this) + File.separator + "sdf_test.png"
    }

    private val ttfPath by lazy {
        StorageHelper.getDownloadPath(this) + File.separator + "DroidSansFallback.ttf"
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val binding = ActivityMainBinding.inflate(layoutInflater)

        setContentView(binding.root)

        TextEngineJni.TEXT_ENGINE_ID = TextEngineJni.initTextEngine()


        binding.btnCreateSdf.setOnClickListener {
            val intent = Intent(this, SdfShaderActivity::class.java)
            intent.putExtra("path", sdfPath)
            startActivity(intent)
        }

        binding.btnTextFont.setOnClickListener {

        }
        binding.btnInset.setOnClickListener {
            val intent = Intent(this, TextureManagerActivity::class.java)
            intent.putExtra("path", ttfPath)
            startActivity(intent)
        }

        binding.btnEdit.setOnClickListener {
            val intent = Intent(this, TextShaderActivity::class.java)
            intent.putExtra("path", ttfPath)
            startActivity(intent)
        }
        binding.btnTextEdit.setOnClickListener {
            val intent = Intent(this, TextEditActivity::class.java)
            intent.putExtra("path", ttfPath)
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

        StorageHelper.copyAssetToPath(this, "sdf_test.png", sdfPath)
        StorageHelper.copyAssetToPath(this, "DroidSansFallback.ttf", ttfPath)
    }
}