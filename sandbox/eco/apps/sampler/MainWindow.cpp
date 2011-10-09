#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include "random.hpp"
#include "geometry.hpp"
#include "statistics.hpp"
#include "util.hpp"

#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::updateOutputWindow()
{
  // Don't update until the user's given an input file.
  if(ui->populationEdit->text().isEmpty())
    return;

  // Start by clearing the output.
  ui->outputText->clear();

  QString str = "<b>Output location</b>:<br/>" + outputPath + "<br/>"
                "<b>Generated files</b>:<br/>";
  QString weight = ui->weightsEdit->text();
  if(ui->centroidalCheck->isChecked()) {
    if(!weight.isEmpty())
      str += outputBase + "-weighted-centroid.txt<br/>";
    else
      str += outputBase + "-centroid.txt<br/>";
  }
  if(ui->nearestCheck->isChecked()) {
    if(!weight.isEmpty())
      str += outputBase + "-weighted-neighbor.txt<br/>";
    else
      str += outputBase + "-neighbor.txt<br/>";
  }
  if(ui->hullCheck->isChecked()) {
    str += outputBase + "-hull.txt<br/>";
  }
  ui->outputText->setText(str);
}

bool MainWindow::readPopulation(Data& data)
{
  // Open the input file.
  std::ifstream f(ui->populationEdit->text().toLatin1());
  if(!f) {
    QMessageBox::critical(this, "Input Error", "Could not open population file");
    return false;
  }

  // Try to read the input matrix.
  read_matrix(f, data);
  if(f.bad()) {
    QMessageBox::critical(this, "Input Error", "Could not read population file");
    return false;
  }
  return true;
}

bool MainWindow::readWeights(Vector& weights)
{
  // Open the input file.
  std::ifstream f(ui->weightsEdit->text().toLatin1());
  if(!f) {
    QMessageBox::critical(this, "Input Error", "Could not open weights file");
    return false;
  }

  // Try to read the input matrix.
  read_vector(f, weights);
  if(f.bad()) {
    QMessageBox::critical(this, "Input Error", "Could not read weights file");
    return false;
  }
  return true;
}

int MainWindow::sampleMethod() const
{
  return ui->samplingChoice->currentIndex();
}

int MainWindow::numSamples() const
{
  return ui->samplesEdit->text().toInt();
}

bool MainWindow::useWeights() const
{
  return !ui->weightsEdit->text().isEmpty();
}

bool MainWindow::computeCentroid() const
{
  return ui->centroidalCheck->isChecked();
}

bool MainWindow::computeNeighbor() const
{
  return ui->nearestCheck->isChecked();
}

bool MainWindow::computeHull() const
{
  return false;
}

template<typename Vec>
  struct WeightedEuclideanDistance
  {
    WeightedEuclideanDistance(Vec const& w)
      : w(w)
    { }

    double operator()(Vec const& a, Vec const& b) const
    {
      return origin::weighted_euclidean_distance(a, b, w);
    }

    Vec const& w;
  };

void MainWindow::generateData()
{
  // Get data, returning if there's an error.
  Data data;
  if(!readPopulation(data))
    return;

  // Get weights. If the weights vector is empty after the operation,
  // return.
  Vector weights;
  if(!ui->weightsEdit->text().isEmpty())
    readWeights(weights);
  else
    weights = Vector(data[0].size(), 1);
  if(weights.empty())
    return;

  // Actually generate and measure data.
  if(sampleMethod() == 0)
    generateSamples(data, weights);

  QMessageBox::information(this, "", "Finished");
}

QString MainWindow::measureFile(QString const& measure)
{
  QString result = outputPath + "/" + outputBase;
  if(!ui->weightsEdit->text().isEmpty())
    result += "-weighted";
  result += "-" + measure + ".txt";
  return result;
}

void MainWindow::computeCentroid(Data const& sample,
                                 Vector const& weights,
                                 std::ofstream& os)
{
  WeightedEuclideanDistance<Vector> dist(weights);

  // Compute distance to center for each element in the sample and observe
  // summary statistics.
  Vector cd(sample.size());
  Vector c = origin::centroid(sample.begin(), sample.end());
  origin::distance_to(sample.begin(), sample.end(), c, cd.begin(), dist);

  // Compute summary statistics
  double min = *std::min_element(cd.begin(), cd.end());
  double max = *std::max_element(cd.begin(), cd.end());
  double mean = origin::arithmetic_mean(cd.begin(), cd.end());
  double sd = origin::sample_standard_deviation(cd.begin(), cd.end(), mean);

  os << sample.size() << " " << min << " " << max << " " << mean << " " << sd << "\n";
}

void MainWindow::computeNeighbor(Data const& sample,
                                 Vector const& weights,
                                 std::ofstream& os)
{
  WeightedEuclideanDistance<Vector> dist(weights);

  // Compute nearest neighbor distances for each element in the sample,
  // and observe summary.
  Vector nnd(sample.size());
  origin::nearest_neighbor_distances(sample.begin(), sample.end(), nnd.begin(), dist);

  // Observe summary statistics
  double min = *std::min_element(nnd.begin(), nnd.end());
  double max = *std::max_element(nnd.begin(), nnd.end());
  double mean = origin::arithmetic_mean(nnd.begin(), nnd.end());
  double sd = origin::sample_standard_deviation(nnd.begin(), nnd.end(), mean);

  os << sample.size() << " " << min << " " << max << " " << mean << " " << sd << "\n";
}

void MainWindow::generateSamples(Data const& data, Vector const& weights)
{
  std::ofstream cf;
  if(computeCentroid()) {
    cf.open(measureFile("centroid").toLatin1());
    if(!cf) {
      QMessageBox::critical(this, "Output Error", "Could not open output file");
      return;
    }
  }

  std::ofstream nf;
  if(computeCentroid()) {
    nf.open(measureFile("neighbor").toLatin1());
    if(!nf) {
      QMessageBox::critical(this, "Output Error", "Could not open output file");
      return;
    }
  }

  // Distance and samples

  // Print header informaiton.
  if(computeCentroid())
    cf << "size min max mean dev\n";
  if(computeNeighbor())
    nf << "size min max mean dev\n";

  // Generate samples, etc.
  for(size_t i = 3; i < data.size(); ++i) {
    for(int j = 0; j < numSamples(); ++j) {
      // Choose a random sample of size i
      Data sample(i);
      origin::random_sample(data.begin(), data.end(), sample.begin(), i, randomGen);

      // Actually compute measures and accumulate the results.
      if(computeCentroid())
        computeCentroid(sample, weights, cf);
      if(computeNeighbor())
        computeNeighbor(sample, weights, nf);
    }
  }
}

void MainWindow::on_populationButton_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, "Open Population File");
  if(!file.isEmpty())
    ui->populationEdit->setText(file);
  else
    return;

  // Get the path and base file name.
  QFileInfo info(file);
  outputPath = info.absolutePath();
  outputBase = info.baseName();
  updateOutputWindow();
}

void MainWindow::on_weightsButton_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, "Open Weights File");
  if(!file.isEmpty())
    ui->weightsEdit->setText(file);
  updateOutputWindow();
}

void MainWindow::on_samplingChoice_currentIndexChanged(int index)
{
  // Enable or disable the inputs for sampling.
  if(index==1) {
    ui->samplesLabel->setEnabled(false);
    ui->samplesEdit->setEnabled(false);
  }
  else {
    ui->samplesLabel->setEnabled(true);
    ui->samplesEdit->setEnabled(true);
  }
}

void MainWindow::on_centroidalCheck_toggled(bool)
{
  updateOutputWindow();
}

void MainWindow::on_nearestCheck_toggled(bool)
{
  updateOutputWindow();
}

void MainWindow::on_hullCheck_toggled(bool)
{
  updateOutputWindow();
}

void MainWindow::on_generateButton_clicked()
{
  // Make sure that the user has chosen an input file
  if(ui->populationEdit->text().isEmpty()) {
    QMessageBox::critical(this,
                          "Input Error",
                          "No input file selected");
    return;
  }

  // For now, disable combination sampling.
  if(sampleMethod() == 1) {
    QMessageBox::critical(this,
                          "Sampling Error",
                          "Combination sampling is not currently supported");
    return;
  }


  // Make sure that at least one output is selected
  if(!ui->centroidalCheck->isChecked() &&
     !ui->nearestCheck->isChecked() &&
     !ui->hullCheck->isChecked())
  {
    QMessageBox::critical(this,
                          "Measure Error",
                          "Must select at least one measure to generate");
    return;
  }

  generateData();
}
