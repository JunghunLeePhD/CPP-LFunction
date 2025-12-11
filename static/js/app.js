let chartReal = null;
let chartImag = null;
let chartComplex = null;
let chartCLT = null;

// 1. HELPER: GCD
function gcd(a, b) {
    while (b) {
        [a, b] = [b, a % b];
    }
    return a;
}

// 2. MAIN LOGIC: Handle Modulus Change
function onModulusChange() {
    const qInput = document.getElementById('mod');
    const kSelect = document.getElementById('char-select');

    let q = parseInt(qInput.value);
    if (isNaN(q) || q < 1) {
        q = 1;
        qInput.value = 1;
    }

    const currentK = parseInt(kSelect.value) || 1;
    kSelect.innerHTML = '';

    let charIndex = 1;

    for (let k = 1; k <= q; k++) {
        if (gcd(k, q) === 1) {
            const option = document.createElement('option');

            option.value = charIndex;

            if (k === 1) option.text = `${k} (Principal)`;
            else if (k === q - 1 && q > 2) option.text = `${k}`;
            else option.text = k;

            kSelect.appendChild(option);

            charIndex++;
        }
    }

    let exists = false;
    for (let opt of kSelect.options) {
        if (parseInt(opt.value) === currentK) {
            kSelect.value = currentK;
            exists = true;
            break;
        }
    }
    if (!exists) kSelect.value = 1;

    updateCharts();
}

// 3. CHART & ZOOM FUNCTIONS
function zoomTo(centerT) {
    const startInput = document.getElementById('start');
    const endInput = document.getElementById('end');
    const currentRange = parseFloat(endInput.value) - parseFloat(startInput.value);
    const newRange = currentRange / 4;
    const newStart = centerT - newRange / 2;
    const newEnd = centerT + newRange / 2;
    startInput.value = newStart.toFixed(4);
    endInput.value = newEnd.toFixed(4);
    updateCharts();
}

function resetZoom() {
    document.getElementById('start').value = '0';
    document.getElementById('end').value = '100';
    updateCharts();
}

const commonOptions = {
    responsive: true,
    maintainAspectRatio: false,
    interaction: { mode: 'index', intersect: false },
    scales: { x: { title: { display: true, text: 't' } } },
    animation: false,
    onClick: (e) => {
        const chart = e.chart;
        const points = chart.getElementsAtEventForMode(e, 'nearest', { intersect: false }, true);
        if (points.length) {
            const xValue = chart.scales.x.getValueForPixel(e.x);
            zoomTo(xValue);
        }
    },
};

async function updateCharts() {
    const r = document.getElementById('real').value;
    const start = document.getElementById('start').value;
    const end = document.getElementById('end').value;
    const q = document.getElementById('mod').value;
    const idx = document.getElementById('char-select').value || 1;
    const steps = Math.min((end - start) * 10, 100000);

    const loader = document.getElementById('loading-sign');
    if (loader) loader.style.display = 'block';

    try {
        const response = await fetch(
            `/scan_flint?r=${r}&start=${start}&end=${end}&q=${q}&idx=${idx}&steps=${steps}`
        );
        const data = await response.json();

        if (data.error) {
            console.error(data.error);
            return;
        }

        const points = data.points;
        const labels = points.map((p) => p.t.toFixed(4));
        const realData = points.map((p) => p.real);
        const imagData = points.map((p) => p.imag);

        if (chartReal) chartReal.destroy();
        chartReal = new Chart(document.getElementById('realChart'), {
            type: 'line',
            data: {
                labels: labels,
                datasets: [
                    {
                        label: 'Re(L(s))',
                        data: realData,
                        borderColor: 'rgb(255, 99, 132)',
                        borderWidth: 2,
                        pointRadius: 0,
                    },
                ],
            },
            options: commonOptions,
        });

        if (chartImag) chartImag.destroy();
        chartImag = new Chart(document.getElementById('imagChart'), {
            type: 'line',
            data: {
                labels: labels,
                datasets: [
                    {
                        label: 'Im(L(s))',
                        data: imagData,
                        borderColor: 'rgb(54, 162, 235)',
                        borderWidth: 2,
                        pointRadius: 0,
                    },
                ],
            },
            options: commonOptions,
        });

        if (chartComplex) chartComplex.destroy();
        chartComplex = new Chart(document.getElementById('complexChart'), {
            type: 'line',
            data: {
                labels: labels,
                datasets: [
                    {
                        label: 'Re',
                        data: realData,
                        borderColor: 'rgb(255, 99, 132)',
                        borderWidth: 2,
                        pointRadius: 0,
                    },
                    {
                        label: 'Im',
                        data: imagData,
                        borderColor: 'rgb(54, 162, 235)',
                        borderWidth: 2,
                        pointRadius: 0,
                    },
                ],
            },
            options: commonOptions,
        });
    } catch (error) {
        console.error('Error computing:', error);
    } finally {
        if (loader) loader.style.display = 'none';
    }
}

// 4. INITIALIZATION
document.addEventListener('DOMContentLoaded', onModulusChange);

// 5. KEYBOARD HANDLERS
document.addEventListener('keydown', function (event) {
    const modInput = document.getElementById('mod');
    const endInput = document.getElementById('end');
    const charInput = document.getElementById('char-select');

    const active = document.activeElement;
    if (active.tagName === 'INPUT' && active !== modInput && active !== endInput) {
        return;
    }
    if (event.shiftKey) {
        if (event.key === 'Escape') {
            resetZoom();
            event.preventDefault();
        }

        if (event.key === 'ArrowUp') {
            modInput.stepUp();
            onModulusChange();
            event.preventDefault();
        }
        if (event.key === 'ArrowDown') {
            modInput.stepDown();
            onModulusChange();
            event.preventDefault();
        }
        if (event.key === 'ArrowRight') {
            endInput.stepUp();
            updateCharts();
            event.preventDefault();
        }
        if (event.key === 'ArrowLeft') {
            endInput.stepDown();
            updateCharts();
            event.preventDefault();
        }
    }

    if (event.altKey) {
        if (event.key === 'ArrowDown') {
            if (charInput.selectedIndex > 0) {
                charInput.selectedIndex--;
                updateCharts();
            }
            event.preventDefault();
        }
        if (event.key === 'ArrowUp') {
            if (charInput.selectedIndex < charInput.options.length - 1) {
                charInput.selectedIndex++;
                updateCharts();
            }
            event.preventDefault();
        }
    }
});

function normalPDF(x) {
    return (1 / Math.sqrt(2 * Math.PI)) * Math.exp(-0.5 * x * x);
}

async function runSelbergCLT() {
    const q = document.getElementById('mod').value;
    const idx = document.getElementById('char-select').value || 1;

    // CLT specific inputs
    const start = document.getElementById('clt-start').value;
    const end = document.getElementById('clt-end').value;
    const samples = document.getElementById('clt-samples').value;

    const loader = document.getElementById('loading-sign');
    if (loader) loader.style.display = 'block';

    try {
        // Fetch data from the new C++ route
        const response = await fetch(
            `/selberg_clt?q=${q}&idx=${idx}&start=${start}&end=${end}&samples=${samples}`
        );
        const data = await response.json();

        // Data Processing: Create Histogram Bins
        // We want a range from approx -4 to +4 (Standard Normal mostly fits here)
        const binCount = 40;
        const minVal = -4.0;
        const maxVal = 4.0;
        const step = (maxVal - minVal) / binCount;

        // Initialize bins
        let bins = new Array(binCount).fill(0);
        let labels = [];

        // Create X-axis labels (bin centers)
        for (let i = 0; i < binCount; i++) {
            labels.push((minVal + i * step + step / 2).toFixed(2));
        }

        // Fill bins
        let validPoints = 0;
        data.samples.forEach((pt) => {
            const val = pt.val; // This is the normalized value
            if (val >= minVal && val < maxVal) {
                const binIdx = Math.floor((val - minVal) / step);
                bins[binIdx]++;
                validPoints++;
            }
        });

        // Normalize heights to Probability Density
        // Height = Count / (Total * BinWidth)
        const densityData = bins.map((count) => count / (validPoints * step));

        // Generate Ideal Gaussian Curve for comparison
        const gaussianData = labels.map((x) => normalPDF(parseFloat(x)));

        // Render Chart
        if (chartCLT) chartCLT.destroy();

        const ctx = document.getElementById('cltChart').getContext('2d');
        chartCLT = new Chart(ctx, {
            type: 'bar',
            data: {
                labels: labels,
                datasets: [
                    {
                        type: 'line',
                        label: 'Standard Normal N(0,1)',
                        data: gaussianData,
                        borderColor: 'red',
                        borderWidth: 2,
                        pointRadius: 0,
                        tension: 0.4, // Smooth curve
                        fill: false,
                    },
                    {
                        type: 'bar',
                        label: 'L-Function Distribution',
                        data: densityData,
                        backgroundColor: 'rgba(54, 162, 235, 0.6)',
                        barPercentage: 1.0,
                        categoryPercentage: 1.0, // Make bars touch like a histogram
                    },
                ],
            },
            options: {
                responsive: true,
                interaction: { mode: 'index', intersect: false },
                scales: {
                    x: { title: { display: true, text: 'Normalized Value' } },
                    y: { title: { display: true, text: 'Probability Density' } },
                },
            },
        });
    } catch (error) {
        console.error('CLT Error:', error);
    } finally {
        if (loader) loader.style.display = 'none';
    }
}
