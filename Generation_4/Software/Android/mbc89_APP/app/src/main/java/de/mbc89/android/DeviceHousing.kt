package de.mbc89.android

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.BoxScope
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.ColumnScope
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.Font
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

val HousingGrey = Color(0xFFDADADA)
private val HousingEdge = Color(0xFFBDBDBD)
private val FaceBlack = Color(0xFF232323)
private val ScrewDark = Color(0xFF0E0E0E)
private val ScrewRim = Color(0xFF4A4A4A)
private val WindowDark = Color(0xFF1A1A1A)
private val WindowRim = Color(0xFF3C3C3C)
private val LabelText = Color(0xFF1C1C1C)

private val DigitFont = FontFamily(Font(R.font.digit))

// The real devices are square (10.8 cm x 10.8 cm); the black face covers the upper part.
private const val FACE_HEIGHT_FRACTION = 0.74f
private val LOGO_WIDTH = 150.dp
private val HOUSING_PADDING = 6.dp
private val FACE_PADDING_H = 32.dp

// Positions in housing coordinates for elements that cross the face edge (e.g. the CU knob).
class HousingGeometry(size: Dp) {
    val faceBottom: Dp = HOUSING_PADDING + (size - HOUSING_PADDING) * FACE_HEIGHT_FRACTION
    private val contentLeft = HOUSING_PADDING + FACE_PADDING_H
    private val spacing = (size - (HOUSING_PADDING + FACE_PADDING_H) * 2 -
        PanelGrid.COLUMN_WIDTH * PanelGrid.COLUMNS) / (PanelGrid.COLUMNS - 1)

    fun columnCenter(column: Int): Dp =
        contentLeft + (PanelGrid.COLUMN_WIDTH + spacing) * column + PanelGrid.COLUMN_WIDTH / 2
}
private val LABEL_INSET = 12.dp
private val LABEL_BOTTOM = 22.dp
private val POWERED_BY_SYMBOL = 72.dp

@Composable
fun DeviceHousing(
    label: String,
    size: Dp,
    overlay: @Composable BoxScope.(HousingGeometry) -> Unit = {},
    content: @Composable ColumnScope.() -> Unit,
) {
    Box(modifier = Modifier.size(size)) {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .clip(RoundedCornerShape(6.dp))
                .background(HousingGrey)
                .border(1.dp, HousingEdge, RoundedCornerShape(6.dp))
                .padding(start = HOUSING_PADDING, top = HOUSING_PADDING, end = HOUSING_PADDING),
        ) {
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .weight(FACE_HEIGHT_FRACTION)
                    .clip(RoundedCornerShape(10.dp))
                    .background(FaceBlack),
            ) {
                FaceScrews(Modifier.matchParentSize())
                Column(
                    modifier = Modifier.padding(horizontal = FACE_PADDING_H, vertical = 22.dp),
                    content = content,
                )
            }
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .weight(1f - FACE_HEIGHT_FRACTION)
                    .padding(start = LABEL_INSET, end = LABEL_INSET, bottom = LABEL_BOTTOM),
            ) {
                Text(
                    label,
                    color = LabelText,
                    fontSize = 18.sp,
                    fontWeight = FontWeight.Bold,
                    modifier = Modifier.align(Alignment.BottomStart),
                )
                // Deliberately marks the panel as an MBCAN replica, not an original Maerklin device.
                Row(
                    modifier = Modifier.align(Alignment.BottomEnd),
                    verticalAlignment = Alignment.Bottom,
                ) {
                    Text("powered by", color = LabelText, fontSize = 12.sp)
                    Image(
                        painter = painterResource(R.drawable.mbc_symbol),
                        contentDescription = "MBCAN",
                        modifier = Modifier
                            .padding(start = 8.dp)
                            .size(POWERED_BY_SYMBOL),
                    )
                }
            }
        }
        overlay(HousingGeometry(size))
    }
}

@Composable
fun FaceHeader(
    display: String,
    displayColor: Color = Color.White,
    besideDisplay: @Composable () -> Unit = {},
) {
    Row(
        modifier = Modifier.fillMaxWidth(),
        verticalAlignment = Alignment.Top,
    ) {
        Box(
            modifier = Modifier
                .size(width = 124.dp, height = 84.dp)
                .border(1.dp, WindowRim)
                .background(WindowDark),
            contentAlignment = Alignment.Center,
        ) {
            Text(
                display,
                color = displayColor,
                fontSize = 72.sp,
                fontFamily = DigitFont,
            )
        }
        besideDisplay()
        Box(modifier = Modifier.weight(1f))
        Column(
            modifier = Modifier.padding(top = 6.dp),
            horizontalAlignment = Alignment.End,
        ) {
            Image(
                painter = painterResource(R.drawable.maerklin_logo),
                contentDescription = "märklin",
                modifier = Modifier.width(LOGO_WIDTH),
            )
            Row(
                modifier = Modifier.width(LOGO_WIDTH),
                verticalAlignment = Alignment.Bottom,
            ) {
                Box(
                    modifier = Modifier
                        .weight(1f)
                        .padding(bottom = 4.dp)
                        .height(1.5.dp)
                        .background(Color.White),
                )
                Text(
                    "digital",
                    color = Color.White,
                    fontSize = 13.sp,
                    fontWeight = FontWeight.Bold,
                    modifier = Modifier.padding(start = 4.dp),
                )
            }
        }
    }
}

@Composable
private fun FaceScrews(modifier: Modifier) {
    Canvas(modifier = modifier) {
        val inset = 12.dp.toPx()
        val radius = 4.dp.toPx()
        listOf(
            Offset(inset, inset),
            Offset(size.width - inset, inset),
            Offset(inset, size.height - inset),
            Offset(size.width - inset, size.height - inset),
        ).forEach { center ->
            drawCircle(ScrewRim, radius + 1.dp.toPx(), center)
            drawCircle(ScrewDark, radius, center)
        }
    }
}
