<?php


/**
 * Sends statistics to the stats daemon over UDP
 *
 **/

class StatsD {

  /**
   * Log timing information
   *
   * @param string $stats The metric to in log timing info for.
   * @param float $time The ellapsed time (ms) to log
   * @param float|1 $sampleRate the rate (0-1) for sampling.
   **/
  public static function timing($stat, $time, $sample_rate = NULL) {
    self::send(array($stat => "$time|ms"), $sample_rate);
  }

  /**
   * Increments one or more stats counters
   *
   * @param string|array $stats The metric(s) to increment.
   * @param float|1 $sampleRate the rate (0-1) for sampling.
   * @return boolean
   **/
  public static function increment($stats, $sample_rate = NULL) {
    self::updateStats($stats, 1, $sample_rate);
  }

  /**
   * Decrements one or more stats counters.
   *
   * @param string|array $stats The metric(s) to decrement.
   * @param float|1 $sampleRate the rate (0-1) for sampling.
   * @return boolean
   **/
  public static function decrement($stats, $sample_rate = NULL) {
    self::updateStats($stats, -1, $sample_rate);
  }

  /**
   * Updates one or more stats counters by arbitrary amounts.
   *
   * @param string|array $stats The metric(s) to update. Should be either a string or array of metrics.
   * @param int|1 $delta The amount to increment/decrement each metric by.
   * @param float|1 $sampleRate the rate (0-1) for sampling.
   * @return boolean
   **/
  public static function updateStats($stats, $delta = 1, $sample_rate = NULL) {

    $data = array();

    if (!is_array($stats)) {
      $stats = array($stats);
    }

    foreach($stats as $stat) {
      $data[$stat] = "$delta|c";
    }

    StatsD::send($data, $sample_rate);

  }

  /*
   * Squirt the metrics over UDP
   **/
  public static function send($data, $sample_rate = NULL) {

    $sample_rate  = $sample_rate ? $sample_rate : variable_get('statsd_sample_rate', 1);
    $sampled_data = array();

    if ($sample_rate < 1) {
      foreach ($data as $stat => $value) {
        if ((mt_rand() / mt_getrandmax()) <= $sample_rate) {
          $sampled_data[$stat] = "$value|@$sample_rate";
        }
      }
    } else {
      $sampled_data = $data;
    }

    if (empty($sampled_data) ) {
      return;
    }

    // Wrap this in a try/catch - failures in any of this should be silently ignored
    try {
      $host = variable_get('statsd_host', '127.0.0.1');
      $port = variable_get('statsd_port', 8125);
      $fp   = fsockopen("udp://$host", $port, $errno, $errstr);

      if ($fp) {
        foreach ($sampled_data as $stat => $value) {
          fwrite($fp, "$stat:$value");
        }
        fclose($fp);
      }

    } catch (Exception $e) {
    }

  }

}