import QtQuick 2.12

Rectangle {
    id: chart
    property string title: ""
    property var series: []
    property var labels: []
    property var colors: ["#5BD7BD", "#E8B66A", "#6EADFF", "#E68CA3", "#AE9BF2", "#D9E4EA"]
    property real rangeStart: 0
    property real rangeEnd: 300
    property var traces: []
    property var latest: UIinterface.manualGripValues
    color: "#15232D"
    radius: 9
    border.color: "#2A3B46"
    function refresh() { refreshTimer.restart() }
    function updatePlot() {
        if (!visible || width < 1) return
        var data = []
        for (var i = 0; i < series.length; ++i)
            data.push(UIinterface.manualGripPlot(series[i], rangeStart, rangeEnd, Math.max(1, Math.floor(width - 105))))
        traces = data
        plot.requestPaint()
    }
    function valueText(index) {
        var v = latest[series[index]]
        return v === null || v === undefined || !isFinite(Number(v)) ? "—" : Number(v).toFixed(series[index] >= 6 ? 0 : 3)
    }
    function timeText(t) { return Math.floor(t / 60) + ":" + ("0" + Math.floor(t % 60)).slice(-2) }
    onSeriesChanged: refresh()
    onRangeStartChanged: refresh()
    onRangeEndChanged: refresh()
    onWidthChanged: refresh()
    onVisibleChanged: refresh()
    Component.onCompleted: refresh()
    Timer { id: refreshTimer; interval: 0; onTriggered: chart.updatePlot() }
    Connections {
        target: UIinterface
        function onManualGripDataChanged() { chart.refresh() }
        function onManualGripReset() { chart.refresh() }
    }
    Text { x: 10; y: 6; text: chart.title; color: "#D6E1E8"; font.pixelSize: 13 }
    Row {
        anchors.top: parent.top; anchors.right: parent.right
        anchors.topMargin: 7; anchors.rightMargin: 10; spacing: 9
        Repeater {
            model: chart.series.length
            Text { text: chart.labels[index] + " " + chart.valueText(index); color: chart.colors[index]; font.pixelSize: chart.width < 850 ? 10 : 12 }
        }
    }
    Canvas {
        id: plot
        objectName: "manualCanvas" + chart.series[0]
        anchors.fill: parent; anchors.topMargin: 27; anchors.bottomMargin: 3
        onHeightChanged: requestPaint()
        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            var left = 86, right = width - 14, top = 6, bottom = height - 18
            if (right <= left || bottom <= top) return
            var low = 0, high = 0, found = false
            for (var s = 0; s < chart.traces.length; ++s) {
                var points = chart.traces[s]
                for (var i = 0; i < points.length; ++i) {
                    var p = points[i]
                    if (!p[2] || !isFinite(p[1])) continue
                    if (!found) { low = high = p[1]; found = true }
                    low = Math.min(low, p[1]); high = Math.max(high, p[1])
                }
            }
            var padding = Math.max(chart.series[0] >= 6 ? 1 : 0.001, (high - low) * 0.12)
            low -= padding; high += padding
            ctx.font = "10px sans-serif"; ctx.textAlign = "right"
            for (var tick = 0; tick < 3; ++tick) {
                var y = top + (bottom - top) * tick / 2
                ctx.strokeStyle = "#2A3B46"; ctx.lineWidth = 1
                ctx.beginPath(); ctx.moveTo(left, y); ctx.lineTo(right, y); ctx.stroke()
                ctx.fillStyle = "#93A8B6"
                ctx.fillText((high - (high - low) * tick / 2).toFixed(chart.series[0] >= 6 ? 0 : 3), left - 5, y + 3)
            }
            ctx.textAlign = "left"; ctx.fillText(chart.timeText(chart.rangeStart), left, height - 3)
            ctx.textAlign = "right"; ctx.fillText(chart.timeText(chart.rangeEnd) + " 分:秒", right, height - 3)
            ctx.save(); ctx.beginPath(); ctx.rect(left, top, right - left, bottom - top); ctx.clip()
            for (s = 0; s < chart.traces.length; ++s) {
                points = chart.traces[s]
                ctx.strokeStyle = chart.colors[s]; ctx.lineWidth = 1.5; ctx.beginPath()
                var connected = false
                for (i = 0; i < points.length; ++i) {
                    p = points[i]
                    if (!p[2] || !isFinite(p[1])) { connected = false; continue }
                    var x = left + (p[0] - chart.rangeStart) / Math.max(0.001, chart.rangeEnd - chart.rangeStart) * (right - left)
                    y = bottom - (p[1] - low) / (high - low) * (bottom - top)
                    if (connected) ctx.lineTo(x, y)
                    else ctx.moveTo(x, y)
                    connected = true
                }
                ctx.stroke()
            }
            ctx.restore()
            if (!found) {
                ctx.textAlign = "center"; ctx.fillStyle = "#93A8B6"
                ctx.fillText("暂无有效数据", (left + right) / 2, (top + bottom) / 2)
            }
        }
    }
}
