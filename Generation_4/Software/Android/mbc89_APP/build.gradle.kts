plugins {
    id("com.android.application") version "8.7.3" apply false
    id("org.jetbrains.kotlin.android") version "2.0.21" apply false
    id("org.jetbrains.kotlin.plugin.compose") version "2.0.21" apply false
}

// OneDrive marks build folders as cloud placeholders that Gradle cannot delete.
val buildRoot = providers.gradleProperty("mbc89.buildRoot").orNull
if (buildRoot != null) {
    allprojects {
        layout.buildDirectory.set(file("$buildRoot/${project.name}"))
    }
}
